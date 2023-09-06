#include <SFML/Graphics.hpp>
#include <thread>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <filesystem>

using namespace sf;
/*
The magic of semikolon:
- Efter deklaration och initialisering av variabler.
- Efter uttryck i uttalanden, som t.ex. efter uttrycken i en for-loop.
*/


class DrawingApp {
public:
    DrawingApp(int initialSizeX = (128*8+20), int initialSizeY = (64*8+20)) : window(VideoMode(initialSizeX, initialSizeY), "Pixel"), numSquares(64), numSquaresTop(128), squareSize(8.0f), sizeX(initialSizeX - 20), sizeY(initialSizeY - 20) {
    initialize();

/*
        createLines();

        createPatterns();

        if (!font.loadFromFile("/System/Library/Fonts/Supplemental/Courier New.ttf")) {
            throw std::runtime_error("Unable to load font");
        }

        createText();
        Color bgColor(85, 107, 47);
        image.create((scale*32)-1, (scale*16)-1, bgColor); 
        memoryImage.create((scale*32)-1, (scale*16)-1, bgColor); 

        selectedBrushSize = scale;
        sprite.setScale(selectedBrushSize, selectedBrushSize); 

        image.copy(memoryImage, 0, 0);

        texture.loadFromImage(image);
        sprite.setTexture(texture, true);

        
        createSquareShapes();
        createTopSquareShapes();

        clock.restart();
        create_menuBar();
        create_dropDown();

        create_infoSpace();

        create_infoSymbols();
        create_infoBrushes();

        isMenuOpen = false; 
        isShiftKeyPressed = false;
        lineStart = Vector2f(-1, -1);
        lineEnd = Vector2f(-1, -1);
        isDrawing = false;
        isErasing = false;
        sizeX = initialSizeX;
        sizeY = initialSizeY;
*/        
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();

            Time elapsedTime = clock.getElapsedTime();
            if (elapsedTime.asSeconds() >= 0.1) {
                render();
                clock.restart();
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            if (isSaveButtonClicked) { 
                Time elapsedTime = saveButtonClock.getElapsedTime();
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
    
    RenderWindow window;
    Image image;
    Texture texture;
    Sprite sprite;
    RectangleShape topLine;
    RectangleShape leftLine;
    RectangleShape leftRuler;
    RectangleShape topRuler;
    RectangleShape leftRulerPattern;
    RectangleShape topRulerPattern;
    Font font;
    Text leftRulerText;
    Text topRulerText;
    Texture rulerPatternTexture;

    
    RectangleShape squareShapes[64];
    RectangleShape squareShapesTop[128];
    RectangleShape brush;
    float scale;
    int sizeX;
    int sizeY;
    float selectedBrushSize;
    Image memoryImage;
    
    
    RectangleShape menuBar;
    RectangleShape dropDown;
    Text menuFile;
    Text menuText;
    Text menuSaveText;
    bool isMenuOpen;
    bool isShiftKeyPressed;
    RectangleShape menuBackground;
    bool isSaveButtonClicked;
    Clock saveButtonClock;

    bool isDrawing;
    bool isErasing;
    bool isMousedown;
    const int numSquares;
    const int numSquaresTop;
    std::vector<Text> leftRulerTexts;
    std::vector<Text> topRulerTexts;
    const float squareSize;
    Clock clock;
    Vector2f lineStart;
    Vector2f lineEnd;
    Color bgColor;
    std::vector<VertexArray> lines;

    RectangleShape infoSpace;
    Text infoSymbolErase;
    RectangleShape brushSize;
    RectangleShape brushSizeSmall;
    RectangleShape brushSizeMed;
    RectangleShape brushSizeMax;

    Color penColor = Color::Black;
    int brushSizeValue = 1;

    bool directoryExists(const std::string& path) {
        struct stat info;
        return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
    }

    void initialize() {
        createLines();
        createPatterns();

         if (!font.loadFromFile("/System/Library/Fonts/Supplemental/Courier New.ttf")) {
            throw std::runtime_error("Unable to load font");
        }
        sizeX = (window.getSize().x)-20;
        sizeY = (window.getSize().y)-20;

        createText();
        Color bgColor(85, 107, 47);
        image.create((scale*32)-1, (scale*16)-1, Color::Transparent); 

        texture.loadFromImage(image);
        sprite.setTexture(texture, true);

        createSquareShapes();
        createTopSquareShapes();

        create_menuBar();
        create_dropDown();

        create_infoSpace();
        create_infoSymbols();
        create_infoBrushes();

        updateBrush();

        isMenuOpen = false;
        isShiftKeyPressed = false;
        lineStart = Vector2f(-1, -1);
        lineEnd = Vector2f(-1, -1);
        isDrawing = false;
        isErasing = false;

        texture.loadFromImage(image);
        sprite.setTexture(texture, true);
        sprite.setScale(scale, scale);

        clock.restart();
    }

    void create_menuBar() {
        menuBar.setSize(Vector2f(static_cast<float>(window.getSize().x), 20));
        menuBar.setFillColor(Color::White);
        menuBar.setPosition(0, 0);

        create_menuText();
    }

    void create_menuText() {
        menuFile.setFont(font);
        menuFile.setCharacterSize(12);
        menuFile.setString("File");
        menuFile.setPosition(20, 2);
        menuFile.setFillColor(Color::Black);
    }

    void create_dropDown() {
        dropDown.setSize(Vector2f(50, 20));
        dropDown.setFillColor(Color::White);
        dropDown.setPosition(15, 20);

        menuSaveText.setFont(font);
        menuSaveText.setCharacterSize(12);
        menuSaveText.setFillColor(Color::Black);
        menuSaveText.setString("Save");
        menuSaveText.setPosition(20, 22);
    }

    void createLines() {
        topLine.setSize(Vector2f(scale*128, 1));
        topLine.setFillColor(Color(60, 60, 60));
        topLine.setPosition(scale*6, scale*11);

        leftLine.setSize(Vector2f(1, scale*64));
        leftLine.setFillColor(Color(60, 60, 60));
        leftLine.setPosition(scale*6, scale*11);

        leftRuler.setSize(Vector2f(24, scale*64));
        leftRuler.setFillColor(Color::Transparent);
        leftRuler.setPosition(1, (scale*11)-1);

        topRuler.setSize(Vector2f(scale*128, 24));
        topRuler.setFillColor(Color::Transparent);
        topRuler.setPosition((scale*6)-1, (scale*5)+1);
    }

    void createPatterns() {
        leftRulerPattern.setSize(Vector2f(24, scale*64));
        leftRulerPattern.setFillColor(Color::Transparent);

        topRulerPattern.setSize(Vector2f(scale*128, 24));
        topRulerPattern.setFillColor(Color::Transparent);
    }

    void createText() {
        leftRulerText.setFont(font);
        leftRulerText.setCharacterSize(12);
        leftRulerText.setFillColor(Color::Black);

        topRulerText.setFont(font);
        topRulerText.setCharacterSize(12);
        topRulerText.setFillColor(Color::Black);

        for (int i = 0; i <= 64; i += 8) {
            Text scaleText(std::to_string(i), font, 12);
            scaleText.setFillColor(Color::Black);

            scaleText.setPosition(scale, (scale*6)+scale + (i*scale) - (scale*2)+(scale*5));
            leftRulerTexts.push_back(scaleText);
        }
        for (int i = 0; i <= 128; i += 8) {
            Text scaleText(std::to_string(i), font, 12);
            scaleText.setFillColor(Color::Black);

            scaleText.setPosition((scale*6)+scale + (i*scale) - (scale*2), 1+(scale*5));
            topRulerTexts.push_back(scaleText);
        }
    }

    void createSquareShapes() {
        for (int i = 0; i < numSquares; ++i) {
            squareShapes[i].setSize(Vector2f(squareSize, squareSize));
            if (i % 2 == 0) {
                squareShapes[i].setFillColor(Color(60, 60, 60));
            } else {
                squareShapes[i].setFillColor(Color(85, 107, 47));
            }
            squareShapes[i].setPosition(scale*5, (scale*11)+1 + (i * (squareSize + 1)));
        }
    }

    void createTopSquareShapes() {
        for (int i = 0; i < numSquaresTop; ++i) {
            squareShapesTop[i].setSize(Vector2f(squareSize, squareSize));
            if ((i - numSquares) % 2 == 0) {
                squareShapesTop[i].setFillColor(Color(60, 60, 60));
            } else {
                squareShapesTop[i].setFillColor(Color(85, 107, 47));
            }
            squareShapesTop[i].setPosition((scale*6)+1 + (i * (squareSize + 1)), scale*10);
        }
    }

    void create_infoSpace() {
        infoSpace.setSize(Vector2f(scale*8, scale*16));
        infoSpace.setPosition(static_cast<float>(window.getSize().x)-(scale*11), (scale*11));

        infoSpace.setFillColor(Color::Transparent);
    }

    void create_infoSymbols() {
        infoSymbolErase.setFont(font);
        infoSymbolErase.setCharacterSize(16);
        infoSymbolErase.setFillColor(Color::Black);
        infoSymbolErase.setString("X");
        infoSymbolErase.setPosition(infoSpace.getPosition().x + (scale*3), infoSpace.getPosition().y);
    }

    void create_infoBrushes() {
        brushSize.setSize(Vector2f(scale*8, scale*32));
        brushSize.setPosition(infoSpace.getPosition().x + (scale*3), infoSpace.getPosition().y);

        brushSize.setFillColor(Color::Transparent);

        brushSizeSmall.setSize(Vector2f(scale, scale));
        brushSizeSmall.setPosition(brushSize.getPosition().x, infoSpace.getPosition().y + 24 + scale);
        brushSizeSmall.setFillColor(Color::Black);

        brushSizeMed.setSize(Vector2f(scale * 2, scale * 2));
        brushSizeMed.setPosition(brushSize.getPosition().x, infoSpace.getPosition().y + 24 + scale * 4);
        brushSizeMed.setFillColor(Color::Black);

        brushSizeMax.setSize(Vector2f(scale * 3, scale * 3));
        brushSizeMax.setPosition(brushSize.getPosition().x, infoSpace.getPosition().y + 24 + scale * 8);
        brushSizeMax.setFillColor(Color::Black);
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

    void drawGridLines(RenderWindow& window) {

        Color gridColor2(80, 100, 40);

        for (int x = (scale*6)+1; x <= (scale*134); x += scale) {
            VertexArray line2(Lines, 2);
            line2[0].position = Vector2f(x, scale*11);
            line2[1].position = Vector2f(x, (64 + 11)*scale);
            line2[0].color = gridColor2;
            line2[1].color = gridColor2;
            window.draw(line2);
        }

        for (int y = (scale*6)+1; y <= scale*(64 + 6); y += scale) {
            VertexArray line2(Lines, 2);
            line2[0].position = Vector2f(scale*6, y+(scale*5));
            line2[1].position = Vector2f(scale*(128 + 6), y+(scale*5));
            line2[0].color = gridColor2;
            line2[1].color = gridColor2;
            window.draw(line2);
        }
        Color gridColor(90, 110, 70);

        for (int x = (scale * 6)+1; x <= scale * (128 + 6); x += scale * 8) {
            VertexArray line(Lines, 2);
            line[0].position = Vector2f(x, (scale * 6) + (scale * 5));
            line[1].position = Vector2f(x, (scale * 64) + (scale * 6) + (scale * 5));
            line[0].color = gridColor;
            line[1].color = gridColor;
            window.draw(line);
        }

        for (int y = (scale * 6)+1; y <= scale * (64 + 6); y += scale * 8) {
            VertexArray line(Lines, 2);
            line[0].position = Vector2f((scale * 6), y + (scale * 5));
            line[1].position = Vector2f((scale * 128) + (scale * 6), y + (scale * 5));
            line[0].color = gridColor;
            line[1].color = gridColor;
            window.draw(line);
        }
    }


void drawShiftLines() {
    if (isShiftKeyPressed) {
        isDrawing = false;
        int shiftAdjustedStartX = static_cast<int>((lineStart.x - 25 - 20) / scale);
        int shiftAdjustedStartY = static_cast<int>((lineStart.y - 25) / scale);

        int startX = static_cast<int>((lineStart.x - 25 - 20) / scale);
        int startY = static_cast<int>((lineStart.y - 25) / scale);
        int endX = static_cast<int>((lineEnd.x  - 25 - 20) / scale);
        int endY = static_cast<int>((lineEnd.y - 25) / scale);

        while (startX != endX || startY != endY) {
            VertexArray currentSegment(Quads, 4);

            currentSegment[0].position = Vector2f(startX, startY);
            currentSegment[1].position = Vector2f(startX + (scale - 1), startY);
            currentSegment[2].position = Vector2f(startX + (scale - 1), startY + (scale - 1));
            currentSegment[3].position = Vector2f(startX, startY + (scale - 1));

            for (int i = 0; i < 4; ++i) {
                currentSegment[i].color = Color::Black;
            }

            lines.push_back(currentSegment);

            if (startX < endX) {
                startX += scale;
            } else if (startX > endX) {
                startX -= scale;
            }
            if (startY < endY) {
                startY += scale;
            } else if (startY > endY) {
                startY -= scale;
            }
        }

        lineStart = Vector2f(-1, -1);
        lineEnd = Vector2f(-1, -1);
    }
}

/* Städa städa städa...
    void handleEvents() {
        Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case Event::Closed:
                    window.close();
                    break;

                    
                case Event::KeyPressed:
                    if (event.key.code == Keyboard::LShift || event.key.code == Keyboard::RShift) {
                        isShiftKeyPressed = true;
                        if (isDrawing) {
                            Vector2i mousePos = Mouse::getPosition(window);
                            lineStart = Vector2f(mousePos);
                        }
                    }
                   
                    if (event.key.code == Keyboard::S && isMenuOpen) {
                        saveImageAsCArray();
                        saveImageAsBMP();
                    }
                   
                    if (event.key.code == Keyboard::X){
                        isDrawing = !isDrawing;
                        isErasing = !isErasing;
                        
                    }
                    break;


                case Event::KeyReleased:
                    if (event.key.code == Keyboard::LShift || event.key.code == Keyboard::RShift) {
                        isShiftKeyPressed = false;
                        
                    }
                    break;
                case Event::MouseButtonPressed:
                    if (event.mouseButton.button == Mouse::Left || event.mouseButton.button == Mouse::Right) {
                        if (event.mouseButton.button == Mouse::Left) {
                            isMousedown = true; 
                            isDrawing = true;
                            if (isShiftKeyPressed) {
                                lineStart = Vector2f(event.mouseButton.x, event.mouseButton.y);
                                lineEnd = lineStart;
                            } else if (event.mouseButton.button == Mouse::Left &&
                                menuFile.getGlobalBounds().contains(static_cast<Vector2f>(Mouse::getPosition(window)))) {
                                isMenuOpen = !isMenuOpen;
                                }
                            
                        }
                        if ((event.mouseButton.button == Mouse::Left || event.mouseButton.button == Mouse::Right) &&! menuFile.getGlobalBounds().contains(static_cast<Vector2f>(Mouse::getPosition(window)))) {
                                isMenuOpen = false;
                                }
                        if (event.mouseButton.button == Mouse::Right) {
                            isErasing = true;
                        }
                        if (event.mouseButton.button == Mouse::Left || event.mouseButton.button == Mouse::Right) {
                            Vector2i mousePos = Mouse::getPosition(window);
                        int x = (mousePos.x - 25 - 20) / scale;
                        int y = ((mousePos.y - 25) / scale)-scale;
                        if (x >= 0 && x < sizeX && y >= 0 && y < sizeY) {
                            for (int i = x - brushSizeValue; i <= x + brushSizeValue; ++i) {
                                for (int j = y - brushSizeValue; j <= y + brushSizeValue; ++j) {
                                    if (i >= 0 && i < sizeX && j >= 0 && j < sizeY) {
                                        if (isErasing) {
                                            image.setPixel(i, j, Color::Transparent);
                                        } else {
                                            image.setPixel(i, j, penColor);
                                        }


                                    }
                                }
                            }
                            texture.loadFromImage(image);
                        }
                        }    
                            
                        if (event.mouseButton.button == Mouse::Right) {
                            isErasing = true;
                        }
                   
                       
                       
                            if (event.mouseButton.button == Mouse::Left) {
                            Vector2i mousePos = Mouse::getPosition(window);
                                if (brushSizeSmall.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
                                    brushSizeValue = (1);
                                    updateBrush();
                                } else if (brushSizeMed.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
                                    brushSizeValue = (5);
                                    updateBrush();
                                } else if (brushSizeMax.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
                                    brushSizeValue = (10);
                                    updateBrush();
                                }     
                            }
                    }
                    break;

                case Event::MouseMoved:
                    if (isDrawing) {
                        Vector2i mousePos = Mouse::getPosition(window);
                        int x = (mousePos.x - 25 - 20) / scale;
                        int y = ((mousePos.y - 25) / scale)-scale;
                        if (x >= 0 && x < sizeX && y >= 0 && y < sizeY) {
                            for (int i = x - brushSizeValue; i <= x + brushSizeValue; ++i) {
                                for (int j = y - brushSizeValue; j <= y + brushSizeValue; ++j) {
                                    if (i >= 0 && i < sizeX && j >= 0 && j < sizeY) {
                                        if (isErasing) {
                                            image.setPixel(i, j, Color::Transparent);
                                        } else {
                                            image.setPixel(i, j, penColor);
                                        }


                                    }
                                }
                            }
                            texture.loadFromImage(image);
                        }
                    }    
                    if (isMousedown && isShiftKeyPressed) {
                        lineEnd = Vector2f(event.mouseMove.x, event.mouseMove.y);
                    }
                    break;

                case Event::MouseButtonReleased:
                isDrawing = false;
                isErasing = false;
                isMousedown = false;
                if (event.mouseButton.button == Mouse::Left || event.mouseButton.button == Mouse::Right) {
                    Vector2i mousePos = Mouse::getPosition(window);
                    isDrawing = false;
                    isErasing = false;
                    isMousedown = false;
                    if (event.mouseButton.button == Mouse::Left || event.mouseButton.button == Mouse::Right) {
                        isDrawing = false;
                        isErasing = false;
                        isMousedown = false;

                            if (event.mouseButton.button == Mouse::Left) {
                                Vector2i mousePos = Mouse::getPosition(window);
                                if (menuSaveText.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) { 
                                    saveImageAsCArray();
                                    saveImageAsBMP();
                                    isMenuOpen = false;
                                }
                            }
                        }
                        if (isShiftKeyPressed) {
                            drawShiftLines();
                            *
                            isDrawing = false;

                            int startX = static_cast<int>((lineStart.x - (scale*6)+1) / scale) * scale + (scale*6)+1;
                            int startY = static_cast<int>((lineStart.y - (scale*6)+1) / scale) * scale + (scale*6)+1;
                            int endX = static_cast<int>((lineEnd.x - (scale*6)+1) / scale) * scale + (scale*6)+1;
                            int endY = static_cast<int>((lineEnd.y - (scale*6)+1) / scale) * scale + (scale*6)+1;

                            while (startX != endX || startY != endY) {
                                VertexArray currentSegment(Quads, 4);

                                currentSegment[0].position = Vector2f(startX, startY);
                                currentSegment[1].position = Vector2f(startX + (scale-1), startY);
                                currentSegment[2].position = Vector2f(startX + (scale-1), startY + (scale-1));
                                currentSegment[3].position = Vector2f(startX, startY + (scale-1));

                                for (int i = 0; i < 4; ++i) {
                                    currentSegment[i].color = Color::Black;
                                }

                                lines.push_back(currentSegment); 

                                if (startX < endX) {
                                    startX += scale;
                                } else if (startX > endX) {
                                    startX -= scale;
                                }
                                if (startY < endY) {
                                    startY += scale;
                                } else if (startY > endY) {
                                    startY -= scale;
                                }
                            }

                            lineStart = Vector2f(-1, -1);
                            lineEnd = Vector2f(-1, -1);*
                        }

                    }
                    break;
                default:
                break;
            }
        }
    }
*/
void handleEvents() {
    Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
            case Event::Closed:
                window.close();
                break;

            case Event::KeyPressed:
                handleKeyPressedEvent(event);
                break;

            case Event::KeyReleased:
                handleKeyReleasedEvent(event);
                break;

            case Event::MouseButtonPressed:
                handleMouseButtonPressedEvent(event);
                break;

            case Event::MouseButtonReleased:
                handleMouseButtonReleasedEvent(event);
                break;

            case Event::MouseMoved:
                handleMouseMovedEvent(event);
                break;

            default:
                break;
        }
    }
}

void handleKeyPressedEvent(const Event& event) {
    if (event.key.code == Keyboard::LShift || event.key.code == Keyboard::RShift) {
        isShiftKeyPressed = true;
        if (isDrawing) {
            Vector2i mousePos = Mouse::getPosition(window);
            lineStart = Vector2f(mousePos);
        }
    }
    
}

void handleKeyReleasedEvent(const Event& event) {
    if (event.key.code == Keyboard::LShift || event.key.code == Keyboard::RShift) {
        isShiftKeyPressed = false;
    }
    
}

void handleMouseButtonPressedEvent(const Event& event) {
    if (event.mouseButton.button == Mouse::Left || event.mouseButton.button == Mouse::Right) {
        isMousedown = true;

        if (event.mouseButton.button == Mouse::Left) {
            isDrawing = true;
            if (isShiftKeyPressed) {
                lineStart = Vector2f(event.mouseButton.x, event.mouseButton.y);
                lineEnd = lineStart;
            } else if (event.mouseButton.button == Mouse::Left && menuFile.getGlobalBounds().contains(static_cast<Vector2f>(Mouse::getPosition(window)))) {
                isMenuOpen = !isMenuOpen;
            }
        }

        if ((event.mouseButton.button == Mouse::Left || event.mouseButton.button == Mouse::Right) && !menuFile.getGlobalBounds().contains(static_cast<Vector2f>(Mouse::getPosition(window)))) {
            isMenuOpen = false;
        }

        if (event.mouseButton.button == Mouse::Right) {
            isErasing = true;
        }

        handleBrushSizeClick(event);
    }
}

void handleMouseButtonReleasedEvent(const Event& event) {
    isDrawing = false;
    isErasing = false;
    isMousedown = false;

    if (event.mouseButton.button == Mouse::Left || event.mouseButton.button == Mouse::Right) {
        handleSaveButtonClick(event);

        if (isShiftKeyPressed) {
            drawShiftLines();
            
        }
    }
}

void handleMouseMovedEvent(const Event& event) {
    if (isDrawing) {
        handleDrawing(event);
    }

    if (isMousedown && isShiftKeyPressed) {
        lineEnd = Vector2f(event.mouseMove.x, event.mouseMove.y);
    }
}

void handleBrushSizeClick(const Event& event) {
    if (event.mouseButton.button == Mouse::Left) {
        Vector2i mousePos = Mouse::getPosition(window);
        if (brushSizeSmall.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
            brushSizeValue = 1;
            updateBrush();
        } else if (brushSizeMed.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
            brushSizeValue = 5;
            updateBrush();
        } else if (brushSizeMax.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
            brushSizeValue = 10;
            updateBrush();
        }
    }
}

void handleSaveButtonClick(const Event& event) {
    if (event.mouseButton.button == Mouse::Left) {
        Vector2i mousePos = Mouse::getPosition(window);
        if (menuSaveText.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
            saveImageAsCArray();
            saveImageAsBMP();
            isMenuOpen = false;
        }
    }
}

void handleDrawing(const Event& event) {
    Vector2i mousePos = Mouse::getPosition(window);
    int x = (mousePos.x - 25 - 20) / scale;
    int y = ((mousePos.y - 25) / scale) - scale;

    if (x >= 0 && x < sizeX && y >= 0 && y < sizeY) {
        for (int i = x - brushSizeValue; i <= x + brushSizeValue; ++i) {
            for (int j = y - brushSizeValue; j <= y + brushSizeValue; ++j) {
                if (i >= 0 && i < sizeX && j >= 0 && j < sizeY) {
                    if (isErasing) {
                        image.setPixel(i, j, Color::Transparent);
                    } else {
                        image.setPixel(i, j, penColor);
                    }
                }
            }
        }
        texture.loadFromImage(image);
    }
}


    void updateBrush() {
        
        brush.setSize(Vector2f(brushSizeValue, brushSizeValue));
        brush.setOrigin(brush.getSize() / scale);
    }

    void render() {
        window.clear(Color(85, 107, 47));
        window.draw(menuBar);
        window.draw(menuFile);
        window.draw(topRulerPattern);
        window.draw(leftRulerPattern);

        drawGridLines(window);
        window.draw(topLine);
        window.draw(leftLine);
        sprite.setPosition((scale*6)+1, (scale*11)+1);
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
        
        Vector2i mousePos = Mouse::getPosition(window);
        brush.setPosition(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        window.draw(brush);

        for (const Text& text : leftRulerTexts) {
            window.draw(text);
        }

        for (const Text& text : topRulerTexts) {
            window.draw(text);
        }

        if (isMenuOpen) {
            window.draw(dropDown);
            window.draw(menuSaveText);
        }

        window.draw(infoSpace);
        window.draw(brushSize);
        window.draw(brushSizeSmall);
        window.draw(brushSizeMed);
        window.draw(brushSizeMax);

        if (isErasing) {
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
