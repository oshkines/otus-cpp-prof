#include <async/async.h>
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <fstream>
#include <cstring>
#include <memory>
#include <atomic>

namespace async {

    // ---------- Структура блока команд ----------
    struct CommandBlock {
        std::vector<std::string> commands;
        uint64_t timestamp;      // миллисекунды с эпохи
        std::string suffix;      // для файлов (_1 или _2)
    };

    // ---------- Потокобезопасная очередь ----------
    template<typename T>
    class BlockingQueue {
    public:
        void push(T value) {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(value));
            cond_.notify_one();
        }

        T pop() {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_.wait(lock, [this] { return !queue_.empty() || stop_; });
            if (stop_ && queue_.empty())
                return T{};
            T value = std::move(queue_.front());
            queue_.pop();
            return value;
        }

        void stop() {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_ = true;
            cond_.notify_all();
        }

        bool empty() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.empty();
        }

    private:
        std::queue<T> queue_;
        mutable std::mutex mutex_;
        std::condition_variable cond_;
        bool stop_ = false;
    };

    // ---------- Диспетчер вывода (синглтон) ----------
    class Dispatcher {
    public:
        static Dispatcher* get() {
            static Dispatcher instance;
            return &instance;
        }

        void submitBlock(const CommandBlock& block) {
            console_queue_.push(block);
            file_queue_.push(block);
        }

        void shutdown() {
            console_queue_.stop();
            file_queue_.stop();
            if (log_thread_.joinable())
                log_thread_.join();
            if (file1_thread_.joinable())
                file1_thread_.join();
            if (file2_thread_.joinable())
                file2_thread_.join();
        }

    private:
        Dispatcher() {
            log_thread_ = std::thread(&Dispatcher::logWorker, this);
            file1_thread_ = std::thread(&Dispatcher::fileWorker, this, "_1");
            file2_thread_ = std::thread(&Dispatcher::fileWorker, this, "_2");
        }

        ~Dispatcher() {
            shutdown();
        }

        void logWorker() {
            while (true) {
                CommandBlock block = console_queue_.pop();
                if (block.commands.empty() && console_queue_.empty())
                    break;
                if (!block.commands.empty()) {
                    // Исправление №1: вывод с префиксом "bulk: "
                    std::cout << "bulk: ";
                    for (size_t i = 0; i < block.commands.size(); ++i) {
                        std::cout << block.commands[i];
                        if (i != block.commands.size() - 1)
                            std::cout << ", ";
                    }
                    std::cout << std::endl;
                }
            }
        }

        void fileWorker(const std::string& suffix) {
            while (true) {
                CommandBlock block = file_queue_.pop();
                if (block.commands.empty() && file_queue_.empty())
                    break;
                if (!block.commands.empty()) {
                    // Исправление №2: timestamp как uint64_t (миллисекунды)
                    std::string filename = "bulk" + std::to_string(block.timestamp) + suffix + ".log";
                    std::ofstream file(filename);
                    if (!file.is_open()) {
                        std::cerr << "Не удалось открыть файл " << filename << std::endl;
                        continue;
                    }
                    for (size_t i = 0; i < block.commands.size(); ++i) {
                        file << block.commands[i];
                        if (i != block.commands.size() - 1)
                            file << ", ";
                    }
                    file << std::endl;
                }
            }
        }

        BlockingQueue<CommandBlock> console_queue_;
        BlockingQueue<CommandBlock> file_queue_;
        std::thread log_thread_;
        std::thread file1_thread_;
        std::thread file2_thread_;
    };

    // ---------- Менеджер общего статического пула (синглтон) ----------
    class StaticPoolManager {
    public:
        static StaticPoolManager* get() {
            static StaticPoolManager instance;
            return &instance;
        }

        void setBlockSize(std::size_t size) {
            std::lock_guard<std::mutex> lock(mutex_);
            block_size_ = size;
        }

        void addCommand(const std::string& cmd) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (current_block_.empty()) {
                block_start_time_ = getCurrentTimestamp();
            }
            current_block_.push_back(cmd);
            if (current_block_.size() >= block_size_) {
                flushBlockLocked();
            }
        }

        void flushBlock() {
            std::lock_guard<std::mutex> lock(mutex_);
            flushBlockLocked();
        }

        void flushAll() {
            flushBlock();
        }

    private:
        StaticPoolManager() : block_size_(3) {}

        uint64_t getCurrentTimestamp() {
            auto now = std::chrono::system_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()
            ).count();
            return static_cast<uint64_t>(ms);
        }

        void flushBlockLocked() {
            if (current_block_.empty())
                return;
            CommandBlock block;
            block.commands = std::move(current_block_);
            block.timestamp = block_start_time_;
            Dispatcher::get()->submitBlock(block);
            current_block_.clear();
            block_start_time_ = 0;
        }

        std::size_t block_size_;
        std::vector<std::string> current_block_;
        uint64_t block_start_time_;
        std::mutex mutex_;
    };

    // ---------- Контекст клиента ----------
    class Context {
    public:
        Context(std::size_t block_size)
            : static_block_size_(block_size)
            , in_dynamic_(false)
            , dynamic_depth_(0)
            , ignore_dynamic_(false)
            , local_block_start_time_(0)
        {
            StaticPoolManager::get()->setBlockSize(block_size);
        }

        void processCommand(const std::string& cmd) {
            if (cmd == "{") {
                startDynamic();
                return;
            }
            if (cmd == "}") {
                endDynamic();
                return;
            }

            if (ignore_dynamic_) {
                return;
            }

            if (in_dynamic_) {
                if (local_block_.empty())
                    local_block_start_time_ = getCurrentTimestamp();
                local_block_.push_back(cmd);
            } else {
                StaticPoolManager::get()->addCommand(cmd);
            }
        }

        void flush() {
            // Исправление №3: НЕ вызываем StaticPoolManager::flushBlock()!
            // Только очищаем локальный динамический блок
            if (in_dynamic_) {
                local_block_.clear();
                ignore_dynamic_ = false;
                in_dynamic_ = false;
                dynamic_depth_ = 0;
            }
        }

    private:
        uint64_t getCurrentTimestamp() {
            auto now = std::chrono::system_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()
            ).count();
            return static_cast<uint64_t>(ms);
        }

        void startDynamic() {
            if (in_dynamic_) {
                dynamic_depth_++;
                return;
            }
            // Завершаем текущий статический блок
            StaticPoolManager::get()->flushBlock();
            in_dynamic_ = true;
            dynamic_depth_ = 1;
            ignore_dynamic_ = false;
            local_block_.clear();
            local_block_start_time_ = getCurrentTimestamp();
        }

        void endDynamic() {
            if (!in_dynamic_)
                return;
            if (dynamic_depth_ > 1) {
                dynamic_depth_--;
                return;
            }
            if (ignore_dynamic_) {
                local_block_.clear();
                ignore_dynamic_ = false;
            } else if (!local_block_.empty()) {
                CommandBlock block;
                block.commands = std::move(local_block_);
                block.timestamp = local_block_start_time_;
                Dispatcher::get()->submitBlock(block);
                local_block_.clear();
                local_block_start_time_ = 0;
            }
            in_dynamic_ = false;
            dynamic_depth_ = 0;
        }

        std::size_t static_block_size_;
        bool in_dynamic_;
        int dynamic_depth_;
        bool ignore_dynamic_;
        std::vector<std::string> local_block_;
        uint64_t local_block_start_time_;
    };

    // ---------- Реализация публичных функций ----------
    void* connect(std::size_t block_size) {
        auto context = std::make_shared<Context>(block_size);
        return new std::shared_ptr<Context>(context);
    }

    void receive(void* context, const char* data, std::size_t size) {
        if (!context || !data || size == 0)
            return;

        auto* ctx_ptr = static_cast<std::shared_ptr<Context>*>(context);
        auto& ctx = *ctx_ptr;

        std::string buffer(data, size);
        std::size_t pos = 0;
        while (pos < buffer.size()) {
            std::size_t end = buffer.find('\n', pos);
            std::string line;
            if (end == std::string::npos) {
                line = buffer.substr(pos);
                pos = buffer.size();
            } else {
                line = buffer.substr(pos, end - pos);
                pos = end + 1;
            }
            if (!line.empty()) {
                ctx->processCommand(line);
            }
        }
    }

    void disconnect(void* context) {
        if (!context)
            return;
        auto* ctx_ptr = static_cast<std::shared_ptr<Context>*>(context);
        (*ctx_ptr)->flush();
        delete ctx_ptr;
    }

    // Глобальная функция для завершения работы сервера (вызывается из main)
    void shutdownDispatcher() {
        Dispatcher::get()->shutdown();
    }

}