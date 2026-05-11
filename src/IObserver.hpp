#pragma once

#include <vector>
#include <string>
#include <ctime>

// Интерфейс наблюдателя (паттерн Observer)
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onCommandsReady(const std::vector<std::string>& commands, std::time_t timestamp) = 0;
};