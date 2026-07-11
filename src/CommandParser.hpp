#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>

class CommandParser {
public:
    using CommandCallback = std::function<std::string(const std::vector<std::string>&)>;

    CommandParser();

    void registerCommand(const std::string& name, CommandCallback callback);
    std::string parse(const std::string& command);

private:
    std::unordered_map<std::string, CommandCallback> callbacks_;
    mutable std::mutex mutex_;  // Исправление №2: потокобезопасность
};