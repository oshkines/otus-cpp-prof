#include "statistics.hpp"
#include <sstream>
#include <stdexcept>

void Stats::add(double value) {
    sum += value;
    sum_sq += value * value;
    count++;
}

void Stats::merge(const Stats& other) {
    sum += other.sum;
    sum_sq += other.sum_sq;
    count += other.count;
}

double Stats::mean() const {
    if (count == 0) return 0.0;
    return sum / static_cast<double>(count);
}

double Stats::variance() const {
    if (count == 0) return 0.0;
    double m = mean();
    return (sum_sq / static_cast<double>(count)) - (m * m);
}

double Stats::stddev() const {
    return std::sqrt(variance());
}

std::string Stats::serialize() const {
    std::ostringstream oss;
    oss << sum << ":" << count << ":" << sum_sq;
    return oss.str();
}

Stats Stats::deserialize(const std::string& str) {
    Stats stats;
    std::istringstream iss(str);
    std::string token;
    
    if (std::getline(iss, token, ':')) {
        stats.sum = std::stod(token);
    }
    if (std::getline(iss, token, ':')) {
        stats.count = static_cast<size_t>(std::stoul(token));
    }
    if (std::getline(iss, token, ':')) {
        stats.sum_sq = std::stod(token);
    }
    
    return stats;
}

std::string KeyValue::serialize() const {
    return key + "\t" + stats.serialize();
}

KeyValue KeyValue::deserialize(const std::string& line) {
    KeyValue kv;
    size_t tab_pos = line.find('\t');
    if (tab_pos != std::string::npos) {
        kv.key = line.substr(0, tab_pos);
        kv.stats = Stats::deserialize(line.substr(tab_pos + 1));
    }
    return kv;
}