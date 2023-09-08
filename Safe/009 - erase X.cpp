#include <SFML/Graphics.hpp>
#include <thread>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <filesystem>

/*
The magic of semikolon:
- Efter deklaration och initialisering av variabler.
- Efter uttryck i uttalanden, som t.ex. efter uttrycken i en for-loop.
*/

class DrawingApp {
public:
    DrawingApp(int initialSizeX = 560+20, int initialSizeY = 300+20) : sizeX(initialSizeX), sizeY(initialSizeY), window(sf::VideoMode(initialSizeX, initialSizeY), "Ritprogram") {

        createLines();

        createPatterns();

        if (!font.loadFromFile("/System/Library/Fonts/Supplemental/Courier New.ttf")) {
            throw std::runtime_error("Unable to load font");
        }

        createText();
        sf::Color bgColor(85, 107, 47);
        image.create(134, 66, bgColor);

        sprite.setScale(4.0f, 4.0f); 

        texture.loadFromImage(image);
        sprite.setTexture(texture, true);
        
        createSquareShapes();
        createTopSquareShapes();

        clock.restart();
        create_menuBar();
        create_dropDown();

        create_infoSpace();
        /*
        if (!eraseIconTexture.loadFromFile("erase_icon.png")) {
            throw std::runtime_error("Unable to load erase icon texture"); 
        }
        */
        create_infoSymbols();

        isMenuOpen = false; 
        isShiftKeyPressed = false;
        lineStart = sf::Vector2f(-1, -1);
        lineEnd = sf::Vector2f(-1, -1);
        isDrawing = false;
        isErasing = false;
/*
Ta det senare, en sak åt gången och sånt... Ändra storlek på fönstret och ritytan vore bra sen, men inte viktigt nu.

        void changeWindowSize(int newSizeX, int newSizeY) {
        sizeX = newSizeX;
        sizeY = newSizeY;
        window.setSize(sf::Vector2u(sizeX, sizeY));
    }*/
        
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
                    

                    saveImageAsCArray();
                    saveImageAsBMP();
                }
            }

        }
    }

private:
    sf::RenderWindow window;
    int sizeX;
    int sizeY;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::RectangleShape menuBar;
    sf::RectangleShape dropDown;
    sf::Text menuFile;
    sf::Text menuText;
    sf::Text menuSaveText;
    sf::RectangleShape topLine;
    sf::RectangleShape leftLine;
    sf::RectangleShape leftRuler;
    sf::RectangleShape topRuler;
    sf::RectangleShape leftRulerPattern;
    sf::RectangleShape topRulerPattern;
    sf::Font font;
    sf::Text leftRulerText;
    sf::Text topRulerText;
    const float squareSize = 3.0f;
    sf::RectangleShape squareShapes[64];
    sf::RectangleShape squareShapesTop[128];
    sf::Texture rulerPatternTexture;
    bool isMenuOpen;
    bool isShiftKeyPressed;
    sf::RectangleShape menuBackground;
    bool isSaveButtonClicked;
    sf::Clock saveButtonClock;
    bool isDrawing;
    bool isErasing;
    bool isMousedown;
    const int numSquares = std::extent<decltype(squareShapes)>::value;
    const int numSquaresTop = std::extent<decltype(squareShapesTop)>::value;
    std::vector<sf::Text> leftRulerTexts;
    std::vector<sf::Text> topRulerTexts;

    sf::Clock clock;
    sf::Vector2f lineStart;
    sf::Vector2f lineEnd;
    sf::Color bgColor;
    std::vector<sf::VertexArray> lines;

    sf::RectangleShape infoSpace;
    sf::Text infoSymbolErase;
    

    bool directoryExists(const std::string& path) {
        struct stat info;
        return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
    }

    void create_menuBar() {
        menuBar.setSize(sf::Vector2f(sizeX, 20));
        menuBar.setFillColor(sf::Color::White);
        menuBar.setPosition(0, 0);

        create_menuText();
    }

    void create_menuText() {
        menuFile.setFont(font);
        menuFile.setCharacterSize(12);
        menuFile.setString("File");
        menuFile.setPosition(20, 2);
        menuFile.setFillColor(sf::Color::Black);
    }

    void create_dropDown() {
        dropDown.setSize(sf::Vector2f(50, 20));
        dropDown.setFillColor(sf::Color::White);
        dropDown.setPosition(15, 20);

        menuSaveText.setFont(font);
        menuSaveText.setCharacterSize(12);
        menuSaveText.setFillColor(sf::Color::Black);
        menuSaveText.setString("Save");
        menuSaveText.setPosition(20, 22);
    }

    void createLines() {
        topLine.setSize(sf::Vector2f(512, 1));
        topLine.setFillColor(sf::Color(60, 60, 60));
        topLine.setPosition(24, 44);

        leftLine.setSize(sf::Vector2f(1, 256));
        leftLine.setFillColor(sf::Color(60, 60, 60));
        leftLine.setPosition(24, 44);

        leftRuler.setSize(sf::Vector2f(23, 256));
        leftRuler.setFillColor(sf::Color::Transparent);
        leftRuler.setPosition(1, 43);

        topRuler.setSize(sf::Vector2f(512, 23));
        topRuler.setFillColor(sf::Color::Transparent);
        topRuler.setPosition(23, 21);
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
        leftRulerText.setPosition(1, 44);

        topRulerText.setFont(font);
        topRulerText.setCharacterSize(12);
        topRulerText.setFillColor(sf::Color::Black);
        topRulerText.setPosition(24, 21);

        for (int i = 0; i <= 128; i += 8) {
            sf::Text scaleText(std::to_string(i), font, 12);
            scaleText.setFillColor(sf::Color::Black);
            
            scaleText.setPosition(1, 26 + (i*4) - 8+20);
            leftRulerTexts.push_back(scaleText);

            scaleText.setPosition(26 + (i*4) - 8, 1+20);
            topRulerTexts.push_back(scaleText);
        }
    }

    void createSquareShapes() {
        for (int i = 0; i < numSquares; ++i) {
            squareShapes[i].setSize(sf::Vector2f(squareSize, squareSize));
            if (i % 2 == 0) {
                squareShapes[i].setFillColor(sf::Color(60, 60, 60));
            } else {
                squareShapes[i].setFillColor(sf::Color(85, 107, 47));
            }
            squareShapes[i].setPosition(20, 45 + (i * (squareSize + 1)));
        }
    }

    void createTopSquareShapes() {
        for (int i = 0; i < numSquaresTop; ++i) {
            squareShapesTop[i].setSize(sf::Vector2f(squareSize, squareSize));
            if ((i - numSquares) % 2 == 0) {
                squareShapesTop[i].setFillColor(sf::Color(60, 60, 60));
            } else {
                squareShapesTop[i].setFillColor(sf::Color(85, 107, 47));
            }

            squareShapesTop[i].setPosition(25 + (i * (squareSize + 1)), 40);
        }
    }

    void create_infoSpace() {
        infoSpace.setSize(sf::Vector2f(30, sizeY-50));
        infoSpace.setPosition(sizeX-35, 45);

        infoSpace.setFillColor(sf::Color::Transparent);
        
    }

    void create_infoSymbols() {
        infoSymbolErase.setFont(font);
        infoSymbolErase.setCharacterSize(16);
        infoSymbolErase.setFillColor(sf::Color::Black);
        infoSymbolErase.setString("X");
        infoSymbolErase.setPosition(infoSpace.getPosition().x + 10, infoSpace.getPosition().y + 10);
    /*
        infoSymbolErase.setTexture(eraseIconTexture);
        
        infoSymbolErase.setPosition(infoSpace.getPosition().x + 10, infoSpace.getPosition().y + 10);
        infoSymbolErase.setScale(1.0f, 1.0f);
*/    
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

    void saveImageAsBMP() {
        if (!createDirectory("save")) {
            return; 
        }

        std::string fileName = generateNextFileName("save", "image", ".bmp"); 

        if (!image.saveToFile(fileName)) {
            std::cerr << "Failed to save the image as BMP: " << fileName << std::endl;
        }
    }

    void saveImageAsCArray() { 
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
        for (int y = 0; y < 64; ++y) {
            for (int x = 0; x < 128; ++x) {
                
                
                bool isBlack = (x % 2 == 0 && y % 2 == 0); 
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

    void drawGridLines(sf::RenderWindow& window) {

        sf::Color gridColor2(80, 100, 40);

        for (int x = 25; x <= 512+25; x += 4) {
            sf::VertexArray line2(sf::Lines, 2);
            line2[0].position = sf::Vector2f(x, 45);
            line2[1].position = sf::Vector2f(x, 256 + 25+20);
            line2[0].color = gridColor2;
            line2[1].color = gridColor2;
            window.draw(line2);
        }

        for (int y = 25; y <= 256 + 25; y += 4) {
            sf::VertexArray line2(sf::Lines, 2);
            line2[0].position = sf::Vector2f(25, y+20);
            line2[1].position = sf::Vector2f(512 + 25, y+20);
            line2[0].color = gridColor2;
            line2[1].color = gridColor2;
            window.draw(line2);
        }
        sf::Color gridColor(90, 110, 70);

        for (int x = 25; x <= 512+25; x += 32) {
            sf::VertexArray line(sf::Lines, 2);
            line[0].position = sf::Vector2f(x, 25+20);
            line[1].position = sf::Vector2f(x, 256 + 25+20);
            line[0].color = gridColor;
            line[1].color = gridColor;
            window.draw(line);
        }

        for (int y = 25; y <= 256 + 25; y += 32) {
            sf::VertexArray line(sf::Lines, 2);
            line[0].position = sf::Vector2f(25, y+20);
            line[1].position = sf::Vector2f(512 + 25, y+20);
            line[0].color = gridColor;
            line[1].color = gridColor;
            window.draw(line);
        }
    }

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    
                    if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift) {
                        isShiftKeyPressed = true;
                        if (isDrawing) {
                            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                            lineStart = sf::Vector2f(mousePos);
                            }
                        
                    }
                    if (event.key.code == sf::Keyboard::S && isMenuOpen) {
                        saveImageAsCArray();
                        saveImageAsBMP();
                    }
                    if (event.key.code == sf::Keyboard::X){
                        isDrawing = !isDrawing;
                        isErasing = !isErasing;
                        
                    }
                    break;
                case sf::Event::KeyReleased:
                    if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift) {
                        isShiftKeyPressed = false;
                        
                    }
                    break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right) {
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            isMousedown = true; 
                            isDrawing = true;
                            if (isShiftKeyPressed) {
                                lineStart = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                                lineEnd = lineStart;
                            } else if (event.mouseButton.button == sf::Mouse::Left &&
                                menuFile.getGlobalBounds().contains(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)))) {
                                isMenuOpen = !isMenuOpen;
                                }
                        }
                        if (event.mouseButton.button == sf::Mouse::Right) {
                            isErasing = true;
                        }
                        if (event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right) {
                            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                            int x = (mousePos.x - 25) / 4;
                            int y = (mousePos.y - 25-20) / 4;
                            if (x >= 0 && x < 128 && y >= 0 && y < 64) { 
                                if (isErasing) {
                                    image.setPixel(x, y, sf::Color::Transparent);
                                    
                                } else {
                                    image.setPixel(x, y, sf::Color::Black);
                                }
                                texture.loadFromImage(image);
                            }
                        }    

                        if (event.mouseButton.button == sf::Mouse::Right) {
                            isErasing = true;
                        }
                    }
                    break;
                    
                case sf::Event::MouseMoved:
                    if ((event.mouseButton.button == sf::Mouse::Left || isMousedown) && (isDrawing || isErasing) &&! isShiftKeyPressed) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        int x = (mousePos.x - 25) / 4;
                        int y = (mousePos.y - 25-20) / 4;
                        if (x >= 0 && x < 128 && y >= 0 && y < 64) { 
                            if (isErasing) {
                                image.setPixel(x, y, sf::Color::Transparent);
                                
                            } else {
                                image.setPixel(x, y, sf::Color::Black);
                            }
                            texture.loadFromImage(image);
                        }
                    }
                    if (isMousedown && isShiftKeyPressed) {
                        lineEnd = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
                    }
               
                break;

                case sf::Event::MouseButtonReleased:
                isDrawing = false;
                isErasing = false;
                isMousedown = false;
                if (event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    isDrawing = false;
                    isErasing = false;
                    isMousedown = false;
                    if (event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right) {
                        isDrawing = false;
                        isErasing = false;
                        isMousedown = false;

                            if (event.mouseButton.button == sf::Mouse::Left) {
                                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                                if (menuSaveText.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) { 
                                    saveImageAsCArray();
                                    saveImageAsBMP();
                                    isMenuOpen = false;
                                }
                            }
                        }
                        if (isShiftKeyPressed) {
                            isDrawing = false;

                            int startX = static_cast<int>((lineStart.x - 25) / 4) * 4 + 25;
                            int startY = static_cast<int>((lineStart.y - 25) / 4) * 4 + 25;
                            int endX = static_cast<int>((lineEnd.x - 25) / 4) * 4 + 25;
                            int endY = static_cast<int>((lineEnd.y - 25) / 4) * 4 + 25;

                            while (startX != endX || startY != endY) {
                                sf::VertexArray currentSegment(sf::Quads, 4);

                                currentSegment[0].position = sf::Vector2f(startX, startY);
                                currentSegment[1].position = sf::Vector2f(startX + 3, startY);
                                currentSegment[2].position = sf::Vector2f(startX + 3, startY + 3);
                                currentSegment[3].position = sf::Vector2f(startX, startY + 3);

                                for (int i = 0; i < 4; ++i) {
                                    currentSegment[i].color = sf::Color::Black;
                                }

                                lines.push_back(currentSegment); 

                                if (startX < endX) {
                                    startX += 4;
                                } else if (startX > endX) {
                                    startX -= 4;
                                }
                                if (startY < endY) {
                                    startY += 4;
                                } else if (startY > endY) {
                                    startY -= 4;
                                }
                            }

                            lineStart = sf::Vector2f(-1, -1);
                            lineEnd = sf::Vector2f(-1, -1);
                        }

                    }
                    break;
                default:
                 break;
           
        }
    }
}

    void render() {
        window.clear(sf::Color(85, 107, 47));
        window.draw(menuBar);
        window.draw(menuFile);
        window.draw(topRulerPattern);
        window.draw(leftRulerPattern);

        sprite.setPosition(25, 45);
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

        drawGridLines(window);
        window.draw(topLine);
        window.draw(leftLine);

        for (const auto& storedLine : lines) {
            window.draw(storedLine);
        }

        if (isDrawing && isShiftKeyPressed) {

            if (isDrawing && lineStart == sf::Vector2f(-1, -1)) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                lineStart = sf::Vector2f(mousePos);
            }

            int startX = static_cast<int>((lineStart.x - 25) / 4) * 4 + 25;
            int startY = static_cast<int>((lineStart.y - 25) / 4) * 4 + 25;
            int endX = static_cast<int>((lineEnd.x - 25) / 4) * 4 + 25;
            int endY = static_cast<int>((lineEnd.y - 25) / 4) * 4 + 25;

            while (startX != endX || startY != endY) {
                sf::VertexArray currentSegment(sf::Quads, 4);


                currentSegment[0].position = sf::Vector2f(startX, startY);
                currentSegment[1].position = sf::Vector2f(startX + 3, startY);
                currentSegment[2].position = sf::Vector2f(startX + 3, startY + 3);
                currentSegment[3].position = sf::Vector2f(startX, startY + 3);

                for (int i = 0; i < 4; ++i) {
                    currentSegment[i].color = sf::Color::Black;
                }

                window.draw(currentSegment);


                if (startX < endX) {
                    startX += 4;
                } else if (startX > endX) {
                    startX -= 4;
                }
                if (startY < endY) {
                    startY += 4;
                } else if (startY > endY) {
                    startY -= 4;
                }
            }

        }

            for (const sf::Text& text : leftRulerTexts) {
                window.draw(text);
            }

            for (const sf::Text& text : topRulerTexts) {
                window.draw(text);
            }

        if (isMenuOpen) {
            window.draw(dropDown);
            window.draw(menuSaveText);
        }

        window.draw(infoSpace);
        if (isErasing){
        window.draw(infoSymbolErase);
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
