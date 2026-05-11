#include "CommandReader.hpp"
#include "BlockProcessor.hpp"
#include "BlockHandler.hpp"
#include <iostream>
#include <memory>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: bulk <N>" << std::endl;
        return 1;
    }

    size_t blockSize = std::atoi(argv[1]);
    if (blockSize == 0) {
        std::cerr << "Invalid block size" << std::endl;
        return 1;
    }

    BlockProcessor processor(blockSize);
    auto handler = std::make_shared<BlockHandler>();
    processor.addObserver(handler);

    CommandReader reader;
    reader.setCallback([&processor](const std::string& cmd) {
        processor.processCommand(cmd);
    });

    reader.run();

    // После окончания ввода – принудительно завершаем текущий блок
    processor.flush();

    return 0;
}