#pragma once

#include "IObserver.hpp"
#include <vector>
#include <string>
#include <ctime>

class BlockHandler : public IObserver {
public:
    void onCommandsReady(const std::vector<std::string>& commands, std::time_t timestamp) override;

private:
    void printToConsole(const std::vector<std::string>& commands);
    void saveToFile(const std::vector<std::string>& commands, std::time_t timestamp);
};