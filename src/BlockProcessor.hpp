#pragma once

#include <vector>
#include <string>
#include <memory>
#include <ctime>
#include "IObserver.hpp"

class BlockProcessor {
public:
    explicit BlockProcessor(size_t staticBlockSize);
    void processCommand(const std::string& cmd);
    void addObserver(std::shared_ptr<IObserver> observer);
    void flush();

private:
    void flushCurrentBlock();
    void startDynamicBlock();
    void endDynamicBlock();

    size_t staticBlockSize_;
    std::vector<std::string> currentBlock_;
    std::time_t blockStartTime_;
    int dynamicBlockDepth_;      // уровень вложенности динамических блоков
    bool inDynamicBlock_;        // находимся ли в динамическом блоке
    bool ignoreDynamicBlock_;    // игнорировать ли текущий динамический блок (из-за нехватки закрытия)

    std::vector<std::shared_ptr<IObserver>> observers_;
};