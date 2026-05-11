#include "BlockHandler.hpp"
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <iomanip>

void BlockHandler::onCommandsReady(const std::vector<std::string>& commands, std::time_t timestamp) {
    printToConsole(commands);
    saveToFile(commands, timestamp);
}

void BlockHandler::printToConsole(const std::vector<std::string>& commands) {
    for (size_t i = 0; i < commands.size(); ++i) {
        std::cout << commands[i];
        if (i != commands.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}

void BlockHandler::saveToFile(const std::vector<std::string>& commands, std::time_t timestamp) {
    std::string filename = "bulk" + std::to_string(timestamp) + ".log";
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл " << filename << std::endl;
        return;
    }

    for (size_t i = 0; i < commands.size(); ++i) {
        file << commands[i];
        if (i != commands.size() - 1) {
            file << ", ";
        }
    }
    file << std::endl;
}