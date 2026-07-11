#include "CommandParser.hpp"
#include <sstream>

CommandParser::CommandParser() {}

void CommandParser::registerCommand(const std::string& name, CommandCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    callbacks_[name] = callback;
}

std::string CommandParser::parse(const std::string& command) {
    std::istringstream iss(command);
    std::string cmdName;
    iss >> cmdName;

    if (cmdName.empty()) {
        return "ERR empty command";
    }

    std::lock_guard<std::mutex> lock(mutex_);
    auto it = callbacks_.find(cmdName);
    if (it == callbacks_.end()) {
        return "ERR unknown command: " + cmdName;
    }

    std::vector<std::string> args;
    std::string arg;
    while (iss >> arg) {
        args.push_back(arg);
    }

    return it->second(args);
}