#include <iostream>
#include <string>
#include <map>
#include <iomanip>
#include "statistics.hpp"

int main() {
    std::map<std::string, Stats> aggregated;
    std::string line;
    
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }
        
        KeyValue kv = KeyValue::deserialize(line);
        if (!kv.key.empty()) {
            aggregated[kv.key].merge(kv.stats);
        }
    }
    
    std::cout << std::fixed << std::setprecision(2);
    for (const auto& pair : aggregated) {
        const std::string& key = pair.first;
        const Stats& stats = pair.second;
        std::cout << key 
                  << "\t" << "mean=" << stats.mean() 
                  << "\t" << "variance=" << stats.variance()
                  << "\t" << "stddev=" << stats.stddev()
                  << "\t" << "count=" << stats.count
                  << std::endl;
    }
    
    return 0;
}