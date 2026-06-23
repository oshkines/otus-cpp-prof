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

    // ---------- Вспомогательные структуры ----------
    struct CommandBlock {
        std::vector<std::string> commands;
        std::time_t timestamp;
        std::string suffix; // "_1" или "_2" для файлов
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
                return T{}; // возвращаем пустой объект (будет обработан)
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

    // ---------- Диспетчер (синглтон) ----------
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
                    break; // остановка
                if (!block.commands.empty()) {
                    // Вывод в консоль
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
                    // Формируем имя файла
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

    // ---------- Контекст (обработка команд для одного подключения) ----------
    class Context {
    public:
        Context(std::size_t block_size, Dispatcher* dispatcher)
            : static_block_size_(block_size)
            , dispatcher_(dispatcher)
            , in_dynamic_block_(false)
            , dynamic_depth_(0)
            , ignore_dynamic_block_(false)
            , block_start_time_(0)
        {}

        void processCommand(const std::string& cmd) {
            if (cmd == "{") {
                startDynamicBlock();
                return;
            }
            if (cmd == "}") {
                endDynamicBlock();
                return;
            }

            if (ignore_dynamic_block_)
                return; // игнорируем команды внутри неполного динамического блока

            if (current_block_.empty())
                block_start_time_ = std::time(nullptr);

            current_block_.push_back(cmd);

            if (!in_dynamic_block_ && current_block_.size() >= static_block_size_) {
                flushBlock();
            }
        }

        void flush() {
            // Принудительное завершение (disconnect или EOF)
            if (in_dynamic_block_) {
                // Если данные закончились внутри динамического блока – игнорируем его
                current_block_.clear();
                ignore_dynamic_block_ = false;
                in_dynamic_block_ = false;
                dynamic_depth_ = 0;
            } else if (!current_block_.empty()) {
                flushBlock();
            }
        }

    private:
        void flushBlock() {
            if (current_block_.empty())
                return;
            CommandBlock block;
            block.commands = std::move(current_block_);
            block.timestamp = block_start_time_;
            // suffix будет добавлен в fileWorker
            dispatcher_->submitBlock(block);
            current_block_.clear();
            block_start_time_ = 0;
        }

        void startDynamicBlock() {
            if (in_dynamic_block_) {
                dynamic_depth_++;
                return;
            }
            // Завершаем текущий статический блок
            flushBlock();
            in_dynamic_block_ = true;
            dynamic_depth_ = 1;
            ignore_dynamic_block_ = false;
            block_start_time_ = std::time(nullptr);
        }

        void endDynamicBlock() {
            if (!in_dynamic_block_)
                return;
            if (dynamic_depth_ > 1) {
                dynamic_depth_--;
                return;
            }
            // Закрытие внешнего динамического блока
            if (ignore_dynamic_block_) {
                current_block_.clear();
                ignore_dynamic_block_ = false;
            } else {
                flushBlock();
            }
            in_dynamic_block_ = false;
            dynamic_depth_ = 0;
        }

    private:
        std::size_t static_block_size_;
        Dispatcher* dispatcher_;
        std::vector<std::string> current_block_;
        bool in_dynamic_block_;
        int dynamic_depth_;
        bool ignore_dynamic_block_;
        std::time_t block_start_time_;
    };

    // ---------- Реализация публичных функций ----------
    void* connect(std::size_t block_size) {
        auto* dispatcher = Dispatcher::get();
        auto context = std::make_shared<Context>(block_size, dispatcher);
        // Возвращаем указатель на shared_ptr (храним в динамической памяти)
        return new std::shared_ptr<Context>(context);
    }

    void receive(void* context, const char* data, std::size_t size) {
        if (!context || !data || size == 0)
            return;

        auto* ctx_ptr = static_cast<std::shared_ptr<Context>*>(context);
        auto& ctx = *ctx_ptr;

        // Разбиваем буфер на команды по '\n'
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
            // Игнорируем пустые строки (можно убрать, если нужны)
            if (!line.empty()) {
                ctx->processCommand(line);
            }
        }
    }

    void disconnect(void* context) {
        if (!context)
            return;
        auto* ctx_ptr = static_cast<std::shared_ptr<Context>*>(context);
        (*ctx_ptr)->flush(); // сбрасываем текущий блок
        delete ctx_ptr;      // освобождаем память
    }

} // namespace async