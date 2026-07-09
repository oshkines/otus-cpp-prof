#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <thread>
#include <cstdlib>

#include "DatabaseManager.hpp"
#include "CommandParser.hpp"

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, DatabaseManager& db, CommandParser& parser)
        : socket_(std::move(socket))
        , db_(db)
        , parser_(parser)
    {}

    void start() {
        doRead();
    }

private:
    void doRead() {
        auto self = shared_from_this();
        boost::asio::async_read_until(socket_, buffer_, '\n',
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::istream is(&buffer_);
                    std::string line;
                    std::getline(is, line);
                    if (!line.empty()) {
                        if (line.back() == '\r')
                            line.pop_back();
                        std::string response = processCommand(line);
                        boost::asio::async_write(socket_, boost::asio::buffer(response + "\n"),
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

    std::string processCommand(const std::string& cmd) {
        // Регистрируем обработчики команд прямо здесь
        parser_.registerCommand("INSERT", [this](const std::vector<std::string>& args) -> std::string {
            if (args.size() != 3) {
                return "ERR invalid arguments for INSERT";
            }
            const std::string& table = args[0];
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
            std::string error;
            if (db_.truncate(table, error)) {
                return "OK";
            } else {
                return "ERR " + error;
            }
        });

        parser_.registerCommand("INTERSECTION", [this](const std::vector<std::string>& args) -> std::string {
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

        return parser_.parse(cmd);
    }

    tcp::socket socket_;
    boost::asio::streambuf buffer_;
    DatabaseManager& db_;
    CommandParser& parser_;
};

class Server {
public:
    Server(boost::asio::io_context& io_context, short port)
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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: join_server <port>\n";
        return 1;
    }

    int port = std::atoi(argv[1]);
    if (port <= 0) {
        std::cerr << "Port must be positive integer.\n";
        return 1;
    }

    try {
        boost::asio::io_context io_context;
        Server server(io_context, static_cast<short>(port));

        std::cout << "Server started on port " << port << std::endl;
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}