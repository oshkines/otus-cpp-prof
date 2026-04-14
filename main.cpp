#include "EditorController.h"
#ifdef _WIN32
#include <windows.h>
#endif

// ------------------------------------------------------------
// Настройка консоли (для Windows)
// ------------------------------------------------------------
void setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, "Russian");
#endif
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
}

// Имитация отрисовки интерфейса (View)
void renderUI(EditorController& ctrl) {
    std::cout << "\n--- СОСТОЯНИЕ РЕДАКТОРА ---\n";
    if (auto doc = ctrl.getDoc()) {
        for (const auto& s : doc->getShapes()) s->draw();
    } else {
        std::cout << "Нет открытых документов.\n";
    }
    std::cout << "---------------------------\n";
}

int main() {
    // Переключаем консоль на UTF-8 для корректного вывода русских букв (Windows).
    // #ifdef _WIN32
    //     system("chcp 65001 > nul");
    // #endif    
    // setupConsole();
	#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif
    EditorController controller;

    // Имитируем действия пользователя
    controller.createNewDocument(); // 1. Создание
    controller.addPrimitive(1);     // 2. Добавление Линии
    controller.addPrimitive(2);     // 3. Добавление Круга
    
    renderUI(controller);           // Посмотреть в отладке (View)

    controller.exportToFile("my_draw.vec"); // 4. Экспорт
    controller.deletePrimitive();   // 5. Удаление

    renderUI(controller);           // Проверка после удаления
    
    return 0;
}