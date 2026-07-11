#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <thread>
#include <cstdlib>
#include <csignal>

#include "DatabaseManager.hpp"
#include "CommandParser.hpp"

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, DatabaseManager& db, CommandParser& parser)
        : socket_(std::move(socket))
        , db_(db)
        , parser_(parser)
        , write_buffer_()  // Исправление №5: буфер для ответа
    {
        // Исправление №1: регистрация команд в конструкторе
        registerCommands();
    }

    void start() {
        doRead();
    }

private:
    void registerCommands() {
        parser_.registerCommand("INSERT", [this](const std::vector<std::string>& args) -> std::string {
            if (args.size() != 3) {
                return "ERR invalid arguments for INSERT";
            }
            const std::string& table = args[0];
            if (table != "A" && table != "B") {
                return "ERR invalid table name: " + table;
            }
            int id = std::stoi(args[1]);
            const std::string& name = args[2];

            std::string error;
            if (db_.insert(table, id, name, error)) {
                return "OK";
            } else {
                return "ERR " + error;
            }
        });

        parser_.registerCommand("TRUNCATE", [this](const std::vector<std::string>& args) -> std::string {
            if (args.size() != 1) {
                return "ERR invalid arguments for TRUNCATE";
            }
            const std::string& table = args[0];
            if (table != "A" && table != "B") {
                return "ERR invalid table name: " + table;
            }
            std::string error;
            if (db_.truncate(table, error)) {
                return "OK";
            } else {
                return "ERR " + error;
            }
        });

        // Исправление №6: проверка на лишние аргументы
        parser_.registerCommand("INTERSECTION", [this](const std::vector<std::string>& args) -> std::string {
            if (!args.empty()) {
                return "ERR INTERSECTION takes no arguments";
            }
            std::string error;
            auto rows = db_.intersection(error);
            if (rows.empty() && !error.empty()) {
                return "ERR " + error;
            }
            std::ostringstream oss;
            for (const auto& row : rows) {
                oss << row.id << "," << row.nameA << "," << row.nameB << "\n";
            }
            oss << "OK";
            return oss.str();
        });

        parser_.registerCommand("SYMMETRIC_DIFFERENCE", [this](const std::vector<std::string>& args) -> std::string {
            // Исправление №6: проверка на лишние аргументы
            if (!args.empty()) {
                return "ERR SYMMETRIC_DIFFERENCE takes no arguments";
            }
            std::string error;
            auto rows = db_.symmetricDifference(error);
            if (rows.empty() && !error.empty()) {
                return "ERR " + error;
            }
            std::ostringstream oss;
            for (const auto& row : rows) {
                oss << row.id << "," << row.nameA << "," << row.nameB << "\n";
            }
            oss << "OK";
            return oss.str();
        });
    }

    void doRead() {
        auto self = shared_from_this();
        boost::asio::async_read_until(socket_, buffer_, '\n',
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    std::istream is(&buffer_);
                    std::string line;
                    std::getline(is, line);
                    if (!line.empty()) {
                        if (line.back() == '\r')
                            line.pop_back();
                        std::string response = parser_.parse(line);
                        // Исправление №5: сохраняем ответ в члене класса
                        write_buffer_ = response + "\n";
                        boost::asio::async_write(socket_, boost::asio::buffer(write_buffer_),
                            [this, self](boost::system::error_code ec, std::size_t) {
                                if (!ec) {
                                    doRead();
                                }
                            });
                    } else {
                        doRead();
                    }
                }
            });
    }

    tcp::socket socket_;
    boost::asio::streambuf buffer_;
    std::string write_buffer_;  // Исправление №5: буфер для хранения ответа
    DatabaseManager& db_;
    CommandParser& parser_;
};

class Server {
public:
    Server(boost::asio::io_context& io_context, uint16_t port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
        , db_()
        , parser_()
    {
        doAccept();
    }

private:
    void doAccept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket), db_, parser_)->start();
                }
                doAccept();
            });
    }

    tcp::acceptor acceptor_;
    DatabaseManager db_;
    CommandParser parser_;
};

void signalHandler(int /*signal*/) {
    std::cout << "\nShutting down server..." << std::endl;
    std::exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: join_server <port>\n";
        return 1;
    }

    int portInt = std::atoi(argv[1]);
    if (portInt <= 0 || portInt > 65535) {
        std::cerr << "Port must be between 1 and 65535.\n";
        return 1;
    }

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    try {
        boost::asio::io_context io_context;
        Server server(io_context, static_cast<uint16_t>(portInt));

        std::cout << "Server started on port " << portInt << std::endl;
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}