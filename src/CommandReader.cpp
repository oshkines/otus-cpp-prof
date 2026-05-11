#include "CommandReader.hpp"
#include <iostream>
#include <string>

void CommandReader::setCallback(Callback cb) {
    callback_ = std::move(cb);
}

void CommandReader::run() {
    std::string line;
    while (std::getline(std::cin, line)) {
        if (callback_) {
            callback_(line);
        }
    }
}