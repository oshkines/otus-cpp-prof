#pragma once
#include "Document.h"

class EditorController {
private:
    std::unique_ptr<Document> currentDoc;

public:
    void createNewDocument() {
        currentDoc = std::make_unique<Document>();
        std::cout << "Создан новый документ.\n";
    }

    void importFromFile(const std::string& path) {
        createNewDocument();
        currentDoc->load(path);
    }

    void exportToFile(const std::string& path) {
        if (currentDoc) currentDoc->save(path);
    }

    void addPrimitive(int type) {
        if (!currentDoc) return;
        if (type == 1) currentDoc->addShape(std::make_unique<Line>());
        else if (type == 2) currentDoc->addShape(std::make_unique<Circle>());
    }

    void deletePrimitive() {
        if (currentDoc) currentDoc->removeLastShape();
    }
    
    Document* getDoc() { return currentDoc.get(); }
};