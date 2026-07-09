#pragma once

#include <string>
#include <vector>
#include <functional>

class CommandParser {
public:
    using CommandCallback = std::function<std::string(const std::vector<std::string>&)>;

    CommandParser();

    std::string parse(const std::string& command);

    void registerCommand(const std::string& name, CommandCallback callback);

private:
    std::unordered_map<std::string, CommandCallback> callbacks_;
};