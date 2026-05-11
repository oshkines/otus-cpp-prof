#include "BlockProcessor.hpp"
#include <iostream>
#include <ctime>

BlockProcessor::BlockProcessor(size_t staticBlockSize)
    : staticBlockSize_(staticBlockSize)
    , blockStartTime_(0)
    , dynamicBlockDepth_(0)
    , inDynamicBlock_(false)
    , ignoreDynamicBlock_(false) {}

void BlockProcessor::addObserver(std::shared_ptr<IObserver> observer) {
    observers_.push_back(observer);
}

void BlockProcessor::processCommand(const std::string& cmd) {
    if (cmd == "{") {
        startDynamicBlock();
        return;
    }
    if (cmd == "}") {
        endDynamicBlock();
        return;
    }

    // Если мы в игнорируемом динамическом блоке, не добавляем команды
    if (ignoreDynamicBlock_) {
        return;
    }

    // Если блок ещё не начат, фиксируем время начала
    if (currentBlock_.empty()) {
        blockStartTime_ = std::time(nullptr);
    }

    currentBlock_.push_back(cmd);

    // Если не в динамическом блоке и достигли размера статического блока
    if (!inDynamicBlock_ && currentBlock_.size() >= staticBlockSize_) {
        flushCurrentBlock();
    }
}

void BlockProcessor::flushCurrentBlock() {
    if (currentBlock_.empty()) {
        return;
    }

    for (auto& observer : observers_) {
        observer->onCommandsReady(currentBlock_, blockStartTime_);
    }
    currentBlock_.clear();
    blockStartTime_ = 0;
}

void BlockProcessor::startDynamicBlock() {
    if (inDynamicBlock_) {
        // Вложенный динамический блок – игнорируем, но увеличиваем счётчик
        dynamicBlockDepth_++;
        return;
    }

    // Принудительно завершаем текущий статический блок
    flushCurrentBlock();

    inDynamicBlock_ = true;
    dynamicBlockDepth_ = 1;
    ignoreDynamicBlock_ = false;
    blockStartTime_ = std::time(nullptr);
}

void BlockProcessor::endDynamicBlock() {
    if (!inDynamicBlock_) {
        return; // не в динамическом блоке
    }

    if (dynamicBlockDepth_ > 1) {
        // Это закрытие вложенного блока
        dynamicBlockDepth_--;
        return;
    }

    // Закрытие внешнего динамического блока
    if (ignoreDynamicBlock_) {
        // Блок был помечен как игнорируемый – отбрасываем
        currentBlock_.clear();
        ignoreDynamicBlock_ = false;
    } else {
        flushCurrentBlock();
    }

    inDynamicBlock_ = false;
    dynamicBlockDepth_ = 0;
}

void BlockProcessor::flush() {
    // Принудительное завершение текущего блока (например, при EOF)
    if (inDynamicBlock_) {
        // Если данные закончились внутри динамического блока, игнорируем его
        currentBlock_.clear();
        ignoreDynamicBlock_ = false;
        inDynamicBlock_ = false;
        dynamicBlockDepth_ = 0;
    } else if (!currentBlock_.empty()) {
        flushCurrentBlock();
    }
}