#include <async/async.h>
#include <iostream>
#include <string>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: bulk <N>" << std::endl;
        return 1;
    }

    int N = std::atoi(argv[1]);
    if (N <= 0) {
        std::cerr << "Error: block size must be positive." << std::endl;
        return 1;
    }

    void* ctx = async::connect(static_cast<std::size_t>(N));

    std::string line;
    while (std::getline(std::cin, line)) {
        // Передаём каждую строку как команду с завершающим '\n'
        async::receive(ctx, line.c_str(), line.size());
        // Для имитации паузы (как в задании 7) можно добавить sleep(1)
        // но в тестах это не требуется.
    }

    async::disconnect(ctx);
    return 0;
}