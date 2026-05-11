#pragma once

#include <vector>
#include <functional>
#include <string>

class CommandReader {
public:
    using Callback = std::function<void(const std::string&)>;

    void setCallback(Callback cb);
    void run();

private:
    Callback callback_;
};