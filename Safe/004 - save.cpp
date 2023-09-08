#include <SFML/Graphics.hpp>
#include <thread> 
#include <fstream> 
#include <iostream>
#include <string>
#include <sstream>
#include <sys/stat.h>

class DrawingApp {
public:
    DrawingApp() : window(sf::VideoMode(512 + 25, 256 + 25), "Ritprogram") {
        
        createLines();

        
        createPatterns();

        
        if (!font.loadFromFile("/System/Library/Fonts/Supplemental/Courier New.ttf")) {
            
            throw std::runtime_error("Unable to load font");
        }

        
        createText();

        
        image.create(128, 64, sf::Color(85, 107, 47)); 

        
        sprite.setScale(4.0f, 4.0f);

        
        texture.loadFromImage(image);
        sprite.setTexture(texture, true);

        
        createSquareShapes();
        createTopSquareShapes();

        
        clock.restart();
        createMenu();

        isMenuOpen = false;
    }

    void createMenu() {
        
        menuBackground.setSize(sf::Vector2f(100, 30)); 
        menuBackground.setFillColor(sf::Color(50, 50, 50, 255)); 
        menuBackground.setPosition(30, 50); 

        menuSaveText.setFont(font);
        menuSaveText.setCharacterSize(16);
        menuSaveText.setFillColor(sf::Color::White);
        menuSaveText.setString("Save");
        menuSaveText.setPosition(45, 55); 
        menuSaveText.setFillColor(sf::Color(150, 150, 150));
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();

            
            sf::Time elapsedTime = clock.getElapsedTime();
            if (elapsedTime.asSeconds() >= 0.1) {
                render();
                clock.restart();
            } else {
                
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            if (isSaveButtonClicked) {
                sf::Time elapsedTime = saveButtonClock.getElapsedTime();
                if (elapsedTime.asSeconds() >= 1.0f) { 
                    isSaveButtonClicked = false;
                    isMenuOpen = false; 
                    menuSaveText.setFillColor(sf::Color(150, 150, 150)); 
                    
                    saveAsCArray();
                    saveAsBMP();
                }
            }
        }
    }

private:
    sf::RenderWindow window;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::RectangleShape topLine;
    sf::RectangleShape leftLine;
    sf::RectangleShape leftRuler;
    sf::RectangleShape topRuler;
    sf::RectangleShape leftRulerPattern;
    sf::RectangleShape topRulerPattern;
    sf::Font font;
    sf::Text leftRulerText;
    sf::Text topRulerText;
    const float squareSize = 4.0f;
    const int numSquares = 64;
    const int numSquaresTop = 128;
    sf::Texture rulerPatternTexture;
    sf::Text menuText;
    bool isMenuOpen;
    sf::RectangleShape menuBackground;
    sf::Text menuSaveText;
    bool isSaveButtonClicked;
    sf::Clock saveButtonClock;
    bool isDrawing;
    bool isErasing;
    sf::RectangleShape squareShapes[64];
    sf::RectangleShape squareShapesTop[128];
    
    sf::Clock clock;

    bool directoryExists(const std::string& path) {
            struct stat info;
            return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
        }

    void createLines() {
        topLine.setSize(sf::Vector2f(512, 1));
        topLine.setFillColor(sf::Color::Black);
        topLine.setPosition(24, 24); 

        leftLine.setSize(sf::Vector2f(1, 256));
        leftLine.setFillColor(sf::Color::Black);
        leftLine.setPosition(24, 24); 

        leftRuler.setSize(sf::Vector2f(23, 256));
        leftRuler.setFillColor(sf::Color::Transparent);
        leftRuler.setPosition(1, 23); 

        topRuler.setSize(sf::Vector2f(512, 23)); 
        topRuler.setFillColor(sf::Color::Transparent);
        topRuler.setPosition(23, 1); 
    }

    void createPatterns() {
        leftRulerPattern.setSize(sf::Vector2f(23, 256));
        leftRulerPattern.setFillColor(sf::Color::Transparent);

        topRulerPattern.setSize(sf::Vector2f(512, 23));
        topRulerPattern.setFillColor(sf::Color::Transparent);
    }

    void createText() {
        leftRulerText.setFont(font);
        leftRulerText.setCharacterSize(12);
        leftRulerText.setFillColor(sf::Color::Black);
        leftRulerText.setPosition(1, 24); 

        topRulerText.setFont(font);
        topRulerText.setCharacterSize(12);
        topRulerText.setFillColor(sf::Color::Black);
        topRulerText.setPosition(24, 1); 
    }

    void createSquareShapes() {
        for (int i = 0; i < numSquares; ++i) {
            squareShapes[i].setSize(sf::Vector2f(squareSize, squareSize));
            if (i % 2 == 0) {
                squareShapes[i].setFillColor(sf::Color::Black); 
            } else {
                squareShapes[i].setFillColor(sf::Color(85, 107, 47)); 
            }

            squareShapes[i].setPosition(20, 25 + (i * squareSize));
        }
    }

    void createTopSquareShapes() {
        for (int i = 0; i < numSquaresTop; ++i) {
            squareShapesTop[i].setSize(sf::Vector2f(squareSize, squareSize));
            if ((i - numSquares) % 2 == 0) {
                squareShapesTop[i].setFillColor(sf::Color::Black); 
            } else {
                squareShapesTop[i].setFillColor(sf::Color(85, 107, 47)); 
            }

            squareShapesTop[i].setPosition(25 + (i * squareSize), 20); 
        }
    }

    void createDirectory(const std::string& path) {
        if (!directoryExists(path)) {
            int status = mkdir(path.c_str(), 0777);
            if (status != 0) {
                std::cerr << "Failed to create directory: " << path << std::endl;
            }
        }
    }

        std::string generateFileName(const std::string& folderPath, const std::string& baseName, const std::string& extension) {
        int fileNumber = 0;
        std::string filePath;

        do {
            std::ostringstream oss;
            oss << folderPath << "/" << baseName << "_" << std::setw(3) << std::setfill('0') << fileNumber << extension;
            filePath = oss.str();
            fileNumber++;
        } while (fileExists(filePath));

        return filePath;
    }

    
    bool fileExists(const std::string& filePath) {
        std::ifstream file(filePath);
        return file.good();
    }
            
    void saveAsBMP() {
        
        if (!image.saveToFile("image.bmp")) {
            std::cerr << "Failed to save the image as BMP." << std::endl;
        }
    }

    void saveAsCArray() {
        
        std::ofstream file("image_data.txt");

        if (!file.is_open()) {
            std::cerr << "Failed to open file for saving C array." << std::endl;
            return;
        }

        file << "const unsigned char imageData[] = {" << std::endl;
        for (int y = 0; y < 64; ++y) {
            for (int x = 0; x < 128; ++x) {
                sf::Color pixelColor = image.getPixel(x, y);
                
                if (pixelColor == sf::Color::Black) {
                    file << "1, ";
                } else {
                    file << "0, ";
                }
            }
        }
        file << "};" << std::endl;

        file.close();
    }


    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        isDrawing = true;
                    } else if (event.mouseButton.button == sf::Mouse::Right) {
                        isErasing = true;
                        isMenuOpen = true;
                        
                        menuBackground.setFillColor(sf::Color(50, 50, 50, 255));
                    }
                    break;
                case sf::Event::MouseButtonReleased:
                    if (event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right) {
                        isDrawing = false;
                        isErasing = false;
                        if (isMenuOpen) {
                            
                            if (event.mouseButton.button == sf::Mouse::Left &&
                                !menuBackground.getGlobalBounds().contains(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)))) {
                                isMenuOpen = false;
                                menuBackground.setFillColor(sf::Color::Transparent);
                            }
                            
                            if (event.mouseButton.button == sf::Mouse::Left) {
                                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                                if (menuSaveText.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                                    saveAsCArray();
                                    saveAsBMP();
                                    isMenuOpen = false;
                                    menuBackground.setFillColor(sf::Color::Transparent);
                                }
                            }
                        }
                    }
                    break;
                case sf::Event::MouseMoved:
                    if (isDrawing) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        int x = (mousePos.x - 25) / 4;
                        int y = (mousePos.y - 25) / 4;
                        if (x >= 0 && x < 128 && y >= 0 && y < 64) {
                            if (isErasing) {
                                image.setPixel(x, y, sf::Color::White);
                            } else {
                                image.setPixel(x, y, sf::Color::Black);
                            }
                            texture.loadFromImage(image);
                        }
                    }
                    break;
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::S && isMenuOpen) {
                        saveAsCArray();
                        saveAsBMP();
                    }
                    break;
                default:
                    break;
            }
        }
    }


    void render() {
        window.clear(sf::Color(85, 107, 47));

        window.draw(topRulerPattern);
        window.draw(leftRulerPattern);

        window.draw(topLine);
        window.draw(leftLine);
        sprite.setPosition(25, 25);
        window.draw(sprite);
        window.draw(leftRuler);
        window.draw(topRuler);
        window.draw(leftRulerText);
        window.draw(topRulerText);
        
        for (int i = 0; i < numSquares; ++i) {
            window.draw(squareShapes[i]);
        }

        
        for (int i = 0; i < numSquaresTop; ++i) {
            window.draw(squareShapesTop[i]);
        }

        if (isMenuOpen) {
            window.draw(menuBackground);
            window.draw(menuSaveText);
        }

        window.display();
    }
};

int main() {
    DrawingApp app;

    try {
        app.run();
    } catch (const std::exception& e) {
        
        return -1;
    }

    return 0;
}
