#pragma once

#include <cstddef>

namespace async {

    /**
     * @brief Создаёт новый контекст обработки команд (для одного клиента).
     * @param block_size Размер статического блока.
     * @return Указатель на контекст (неинтерпретируемый для вызывающего кода).
     */
    void* connect(std::size_t block_size);

    /**
     * @brief Передаёт данные (одну или несколько команд) в контекст.
     * @param context Контекст, возвращённый connect().
     * @param data Указатель на буфер с командами.
     * @param size Размер буфера в байтах.
     */
    void receive(void* context, const char* data, std::size_t size);

    /**
     * @brief Завершает работу с контекстом. Все накопленные команды сбрасываются.
     * @param context Контекст, возвращённый connect().
     */
    void disconnect(void* context);

}