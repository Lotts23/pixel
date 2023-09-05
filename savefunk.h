#ifndef SAVEFUNK_H
#define SAVEFUNK_H

#include <SFML/Graphics.hpp> 

class DrawingApp;

class SaveFunk {
public:
    
    SaveFunk(DrawingApp& app);

    
    bool directoryExists(const std::string& path);
    bool createDirectory(const std::string& path);
    std::string generateNextFileName(const std::string& folderPath, const std::string& baseName, const std::string& extension);
    void saveImageAsBMP(const sf::Image& canvas);
    void saveImageAsCArray(const sf::Image& canvas);
    void closeSavemenu(sf::RenderWindow& window, sf::Event& event, bool& isMenuOpen, DrawingApp& drawingApp);


private:
    bool isSaveButtonClicked; 
    DrawingApp& drawingApp;
};

#endif 
