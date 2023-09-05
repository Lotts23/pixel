#include "savefunk.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <filesystem>
/*
Här blev det en dublett, detta ligger redan i savefunk.h

class SaveFunk {
public:
    
    SaveFunk();

    
    bool directoryExists(const std::string& path);
    bool createDirectory(const std::string& path);
    std::string generateNextFileName(const std::string& folderPath, const std::string& baseName, const std::string& extension);
    void saveImageAsBMP(const sf::Image& canvas);
    void saveImageAsCArray(const sf::Image& canvas);
    void closeSavemenu(sf::RenderWindow& window, sf::Event& event, bool& isMenuOpen);

private:
    bool isSaveButtonClicked;
};
*/
SaveFunk::SaveFunk(DrawingApp& app) : drawingApp(app) {
    isSaveButtonClicked = false;
}

bool directoryExists(const std::string& path) {
    struct stat info;
    return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

bool createDirectory(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        int status = mkdir(path.c_str(), 0777);
        if (status != 0) {
            std::cerr << "Failed to create dracony: " << path << std::endl;
            return false;
        }
    }
    return true;
}

std::string generateNextFileName(const std::string& folderPath, const std::string& baseName, const std::string& extension) {
    int fileNumber = 0;
    std::string filePath;

    do { 
        std::ostringstream oss;
        oss << folderPath << "/" << baseName << "_" << std::setw(3) << std::setfill('0') << fileNumber << extension;
        filePath = oss.str();
        fileNumber++;
    } while (std::filesystem::exists(filePath));

    return filePath;
}

void saveImageAsBMP(const sf::Image& canvas) {

    if (!createDirectory("save")) {
        return;
    }

    std::string fileName = generateNextFileName("save", "image", ".bmp");

    if (!canvas.saveToFile(fileName)) {
        std::cerr << "Failed to save the image as BMP: " << fileName << std::endl;
    }
}

void saveImageAsCArray(const sf::Image& canvas) {
    if (!createDirectory("save")) {
        return;
    }

    std::string fileName = generateNextFileName("save", "image_data", ".h");
    std::ofstream file(fileName.c_str());

    if (!file.is_open()) {
        std::cerr << "Failed to open file for saving C array: " << fileName << std::endl;
        return;
    }

    file << "#pragma once" << std::endl;
    file << "const unsigned char imageData[] = {" << std::endl;
    
    for (int y = 0; y < canvas.getSize().y; ++y) {
        for (int x = 0; x < canvas.getSize().x; ++x) {
            sf::Color pixelColor = canvas.getPixel(x, y);
            bool isBlack = (pixelColor == sf::Color::Black);
            if (isBlack) {
                file << "1, ";
            } else {
                file << "0, ";
            }
        }
    }

    file << "};" << std::endl;
    file.close();
}

void SaveFunk::closeSavemenu(sf::RenderWindow& window, sf::Event& event, bool& isMenuOpen, DrawingApp& drawingApp) {
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (drawingApp.menuSaveText.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) { 
            isSaveButtonClicked = true;
            drawingApp.isMenuOpen = false;
        }
    }
}

    
