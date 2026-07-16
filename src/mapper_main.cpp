#include <iostream>
#include <string>
#include "csv_parser.hpp"
#include "statistics.hpp"

int main() {
    std::string line;
    
    while (std::getline(std::cin, line)) {
        if (line.empty() || csv::isHeader(line)) {
            continue;
        }
        
        auto fields = csv::parseLine(line);
        
        if (fields.size() <= 9) {
            continue;
        }
        
        const std::string& price_str = fields[9];
        if (price_str.empty()) {
            continue;
        }
        
        try {
            double price = std::stod(price_str);
            if (price >= 0) {
                Stats stats;
                stats.add(price);
                std::cout << "price" << "\t" << stats.serialize() << std::endl;
            }
        } catch (const std::exception&) {
            // Игнорируем некорректные цены
        }
    }
    
    return 0;
}