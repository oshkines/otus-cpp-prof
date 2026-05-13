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

    int blockSizeInt = std::atoi(argv[1]);
    if (blockSizeInt <= 0) {
        std::cerr << "Error: block size must be a positive integer." << std::endl;
        return 1;
    }
    size_t blockSize = static_cast<size_t>(blockSizeInt);

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