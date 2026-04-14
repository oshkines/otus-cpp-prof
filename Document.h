#pragma once
#include <vector>
#include <string>
#include "Shape.h"

class Document {
private:
    // Храним вектор умных указателей на фигуры
    std::vector<std::unique_ptr<Shape>> shapes;

public:
    void addShape(std::unique_ptr<Shape> shape) {
        shapes.push_back(std::move(shape)); // Передаем владение в вектор
    }

    void removeLastShape() {
        if (!shapes.empty()) shapes.pop_back();
    }

    void save(const std::string& path) { std::cout << "Документ сохранен в: " << path << "\n"; }
    void load(const std::string& path) { std::cout << "Документ загружен из: " << path << "\n"; }

    const std::vector<std::unique_ptr<Shape>>& getShapes() const { return shapes; }
};