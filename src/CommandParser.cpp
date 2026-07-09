#include "CommandParser.hpp"
#include <sstream>
#include <unordered_map>

CommandParser::CommandParser() {
    // Команды регистрируются в main.cpp
}

void CommandParser::registerCommand(const std::string& name, CommandCallback callback) {
    callbacks_[name] = callback;
}

std::string CommandParser::parse(const std::string& command) {
    std::istringstream iss(command);
    std::string cmdName;
    iss >> cmdName;

    if (cmdName.empty()) {
        return "ERR empty command";
    }

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