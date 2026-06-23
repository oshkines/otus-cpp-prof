#pragma once

#include <cstddef>

// Макросы для экспорта/импорта в Windows
#ifdef _WIN32
    #ifdef ASYNC_EXPORT
        #define ASYNC_API __declspec(dllexport)
    #else
        #define ASYNC_API __declspec(dllimport)
    #endif
#else
    #define ASYNC_API
#endif

namespace async {

    ASYNC_API void* connect(std::size_t block_size);
    ASYNC_API void receive(void* context, const char* data, std::size_t size);
    ASYNC_API void disconnect(void* context);

} // namespace async