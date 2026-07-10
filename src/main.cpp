#include <async/async.h>
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <cstdlib>
#include <csignal>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, std::size_t block_size)
        : socket_(std::move(socket))
        , context_(async::connect(block_size))
    {}

    void start() {
        doRead();
    }

    ~Session() {
        async::disconnect(context_);
    }

private:
    void doRead() {
        auto self = shared_from_this();
        boost::asio::async_read_until(socket_, buffer_, '\n',
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    std::istream is(&buffer_);
                    std::string line;
                    std::getline(is, line);
                    if (!line.empty()) {
                        if (!line.empty() && line.back() == '\r')
                            line.pop_back();
                        async::receive(context_, line.c_str(), line.size());
                    }
                    doRead();
                } else {
                    // Ошибка (закрытие соединения) – проверяем остаток в буфере
                    if (buffer_.size() > 0) {
                        std::istream is(&buffer_);
                        std::string line;
                        while (std::getline(is, line)) {
                            if (!line.empty()) {
                                if (!line.empty() && line.back() == '\r')
                                    line.pop_back();
                                async::receive(context_, line.c_str(), line.size());
                            }
                        }
                    }
                }
            });
    }

    tcp::socket socket_;
    boost::asio::streambuf buffer_;
    void* context_;
};

class Server {
public:
    Server(boost::asio::io_context& io_context, uint16_t port, std::size_t block_size)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
        , block_size_(block_size)
    {
        doAccept();
    }

    ~Server() {
        async::shutdownDispatcher();
    }

private:
    void doAccept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket), block_size_)->start();
                }
                doAccept();
            });
    }

    tcp::acceptor acceptor_;
    std::size_t block_size_;
};

// Обработка Ctrl+C
void signalHandler(int /*signal*/) {
    std::cout << "\nShutting down server..." << std::endl;
    async::shutdownDispatcher();
    std::exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: bulk_server <port> <bulk_size>\n";
        return 1;
    }

    int portInt = std::atoi(argv[1]);
    int bulk_size = std::atoi(argv[2]);

    if (portInt <= 0 || portInt > 65535) {
        std::cerr << "Port must be between 1 and 65535.\n";
        return 1;
    }
    if (bulk_size <= 0) {
        std::cerr << "Bulk size must be positive.\n";
        return 1;
    }

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    try {
        // Исправление: создаём io_context сразу в unique_ptr
        auto io_context = std::make_unique<boost::asio::io_context>();
        Server server(*io_context, static_cast<uint16_t>(portInt), static_cast<std::size_t>(bulk_size));

        std::cout << "Server started on port " << portInt << " with bulk size " << bulk_size << std::endl;

        io_context->run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        async::shutdownDispatcher();
        return 1;
    }

    async::shutdownDispatcher();
    return 0;
}