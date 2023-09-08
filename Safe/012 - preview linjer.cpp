#include <SFML/Graphics.hpp>
#include <thread>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <filesystem>
#include <cmath>

using namespace sf;

class DrawingApp {
public:
    DrawingApp() : window(sf::VideoMode(initialSizeX, initialSizeY), "Pixel") {
    initialize();
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
    float scale = 8.0f;
    int initialSizeX = (128 + scale*2 + (scale/2)) * scale;
    int initialSizeY = (64 + scale*2 + (scale/2)) * scale;
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
    RenderTexture renderTexture;

    RectangleShape squareShapes[64];
    RectangleShape squareShapesTop[128];
    RectangleShape brush;
    float selectedBrushSize;
    const int numSquares = std::extent<decltype(squareShapes)>::value;
    const int numSquaresTop = std::extent<decltype(squareShapesTop)>::value;
    const float squareSize = (scale);
    int sizeX = initialSizeX - scale*2 + (scale/2);
    int sizeY = initialSizeY - scale*2 + (scale/2);
    int startPixelX;
    int startPixelY;
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
    std::vector<Text> leftRulerTexts;
    std::vector<Text> topRulerTexts;
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
    float brushSizeValueStart = 1;
    float brushSizeValue = 1;
    float brushSizeValueMed = brushSizeValueStart*2;
    float brushSizeValueMax = brushSizeValueStart*4;
    float t;
    RectangleShape pixel;

    Vector2f liveLineStart;
    Vector2f liveLineEnd;
    bool isDrawingliveLine = false;
    Vector2f lineStartPreview;
    Vector2f lineEndPreview;


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
        sizeX = ((window.getSize().x)- ((scale*2 + (scale/2))*scale))/scale + 1;
        sizeY = ((window.getSize().y)-((scale*2 + (scale/2))*scale))/scale + 1;



        createText();
        Color bgColor(85, 107, 47);
        image.create((sizeX), (sizeY), Color::Transparent);

        texture.loadFromImage(image);
        sprite.setTexture(texture, true);
        sprite.setScale(scale, scale);

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

        isDrawing = false;
        isErasing = false;
        lineStart = Vector2f(-scale, -scale);
        lineEnd = Vector2f(-scale, -scale);
        lineStartPreview = Vector2f(-scale, -scale);
        lineEndPreview = Vector2f(-scale, -scale);

        clock.restart();
    }

    void create_menuBar() {
        menuBar.setSize(Vector2f(static_cast<float>(window.getSize().x), scale*3));
        menuBar.setFillColor(Color::White);
        menuBar.setPosition(0, 0);

        create_menuText();
    }

    void create_menuText() {
        menuFile.setFont(font);
        menuFile.setCharacterSize(12);
        menuFile.setString("File");
        menuFile.setPosition(scale*3, scale/4);
        menuFile.setFillColor(Color::Black);
    }

    void create_dropDown() {
        dropDown.setSize(Vector2f(scale*6, scale*3));
        dropDown.setFillColor(Color::White);
        dropDown.setPosition(scale*2 + (scale/2), scale*3);

        menuSaveText.setFont(font);
        menuSaveText.setCharacterSize(12);
        menuSaveText.setFillColor(Color::Black);
        menuSaveText.setString("Save");
        menuSaveText.setPosition(scale*3, scale*3 + (scale/2));
    }

    void createLines() {
        leftRuler.setSize(Vector2f(scale*3, scale*64));
        leftRuler.setFillColor(Color::Transparent);
        leftRuler.setPosition(0, (scale*11));

        topRuler.setSize(Vector2f(scale*128, scale*3));
        topRuler.setFillColor(Color::Transparent);
        topRuler.setPosition((scale*6), 0);

        topLine.setSize(Vector2f(scale*128, 1));
        topLine.setFillColor(Color(60, 60, 60));
        topLine.setPosition(scale*6, scale*11-1);

        leftLine.setSize(Vector2f(1, scale*64));
        leftLine.setFillColor(Color(60, 60, 60));
        leftLine.setPosition(scale*6-1, scale*11);
    }

    void createPatterns() {
        leftRulerPattern.setSize(Vector2f(scale*3, scale*64));
        leftRulerPattern.setFillColor(Color::Transparent);

        topRulerPattern.setSize(Vector2f(scale*128, scale*3));
        topRulerPattern.setFillColor(Color::Transparent);
    }

    void createText() {
        leftRulerText.setFont(font);
        leftRulerText.setCharacterSize(12);
        leftRulerText.setFillColor(Color::Black);

        topRulerText.setFont(font);
        topRulerText.setCharacterSize(12);
        topRulerText.setFillColor(Color::Black);

        for (int i = 0; i <= 64; i += scale) {
            Text scaleText(std::to_string(i), font, 12);
            scaleText.setFillColor(Color::Black);

            scaleText.setPosition(2*scale, (scale*6)+scale + (i*scale) - (scale*2)+(scale*5));
            leftRulerTexts.push_back(scaleText);
        }
        for (int i = 0; i <= 128; i += scale) {
            Text scaleText(std::to_string(i), font, 12);
            scaleText.setFillColor(Color::Black);

            scaleText.setPosition((scale*6)+scale + (i*scale) - (scale*2), scale+(scale*5));
            topRulerTexts.push_back(scaleText);
        }
    }

    void createSquareShapes() {
        for (int i = 0; i < numSquares; ++i) {
            squareShapes[i].setSize(Vector2f(squareSize, squareSize));
            if (i % 2 == 0) {
                squareShapes[i].setFillColor(Color(60, 60, 60));
            } else {
                squareShapes[i].setFillColor(Color::Transparent);
            }
            squareShapes[i].setPosition((scale*5), ((scale*11) + (i * (squareSize)))-1);
        }
    }

    void createTopSquareShapes() {
        for (int i = 0; i < numSquaresTop; ++i) {
            squareShapesTop[i].setSize(Vector2f(squareSize, squareSize));
            if ((i - numSquares) % 2 == 0) {
                squareShapesTop[i].setFillColor(Color(60, 60, 60));
            } else {
                squareShapesTop[i].setFillColor(Color::Transparent);
            }
            squareShapesTop[i].setPosition(((scale*6) + (i * (squareSize))) - 1, scale*10);
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

        brushSizeSmall.setSize(Vector2f(brushSizeValueStart*scale, brushSizeValueStart*scale));
        brushSizeSmall.setPosition(brushSize.getPosition().x, infoSpace.getPosition().y + (scale*3) + scale);
        brushSizeSmall.setFillColor(Color::Black);

        brushSizeMed.setSize(Vector2f(brushSizeValueMed*scale, brushSizeValueMed*scale));
        brushSizeMed.setPosition(brushSize.getPosition().x, infoSpace.getPosition().y + (scale*3) + scale * (scale/2));
        brushSizeMed.setFillColor(Color::Black);

        brushSizeMax.setSize(Vector2f(brushSizeValueMax*scale, brushSizeValueMax*scale));
        brushSizeMax.setPosition(brushSize.getPosition().x, infoSpace.getPosition().y + (scale*3) + scale * scale);
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
        Color gridColor2(80, 100, 40, 150);

        for (int x = (scale * 6); x <= (scale * 134); x += scale) {
            VertexArray line2(Lines, 2);
            line2[0].position = Vector2f(static_cast<float>(x), scale * 11);
            line2[1].position = Vector2f(static_cast<float>(x), (64 + 11) * scale);
            line2[0].color = gridColor2;
            line2[1].color = gridColor2;
            window.draw(line2);
        }

        for (int y = (scale * 6); y <= scale * (64 + 6); y += scale) {
            VertexArray line2(Lines, 2);
            line2[0].position = Vector2f(scale * 6, static_cast<float>(y + (scale * 5)));
            line2[1].position = Vector2f(scale * (128 + 6), static_cast<float>(y + (scale * 5)));
            line2[0].color = gridColor2;
            line2[1].color = gridColor2;
            window.draw(line2);
        }
        Color gridColor(90, 110, 70);

        for (int x = (scale * 6); x <= scale * (128 + 6); x += scale * 8) {
            VertexArray line(Lines, 2);
            line[0].position = Vector2f(x, (scale * 6) + (scale * 5));
            line[1].position = Vector2f(x, (scale * 64) + (scale * 6) + (scale * 5));
            line[0].color = gridColor;
            line[1].color = gridColor;
            window.draw(line);
        }

        for (int y = (scale * 6); y <= scale * (64 + 6); y += scale * 8) {
            VertexArray line(Lines, 2);
            line[0].position = Vector2f((scale * 6), y + (scale * 5));
            line[1].position = Vector2f((scale * 128) + (scale * 6), y + (scale * 5));
            line[0].color = gridColor;
            line[1].color = gridColor;
            window.draw(line);
        }
    }

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
            lineStart.x = std::round((mousePos.x - 20 - 20) / scale) * scale + 20 + 20;
            lineStart.y = std::round((mousePos.y - 20) / scale) * scale + 20;
                lineStartPreview = Vector2f(event.mouseButton.x, event.mouseButton.y);
                lineEndPreview = lineStartPreview;
        }
    }
     if (event.key.code == Keyboard::X) {
        isErasing = true;
     }
}

void handleKeyReleasedEvent(const Event& event) {
    if (event.key.code == Keyboard::LShift || event.key.code == Keyboard::RShift) {
        isShiftKeyPressed = false;
        pixel.setPosition(Vector2f(-scale, -scale));
        pixel.setFillColor(Color::Transparent);
        
    }
}

void handleMouseButtonPressedEvent(const Event& event) {
    if (event.mouseButton.button == Mouse::Left || event.mouseButton.button == Mouse::Right) {
        isMousedown = true;
        if (event.mouseButton.button == Mouse::Left) {
            isDrawing = true;
            if (isShiftKeyPressed) {
                Vector2i mousePos = Mouse::getPosition(window);
                lineStart.x = std::round((mousePos.x - 20 - 20) / scale) * scale + 20 + 20;
                lineStart.y = std::round((mousePos.y - 20) / scale) * scale + 20;
                lineEnd = lineStart;
                lineStartPreview = Vector2f(event.mouseButton.x, event.mouseButton.y);
                lineEndPreview = lineStartPreview;
                
            } else if (menuFile.getGlobalBounds().contains(static_cast<Vector2f>(Mouse::getPosition(window)))) {
                isMenuOpen = !isMenuOpen;
            } else {
                handleDrawing(event);
            }
        }

        if (!menuFile.getGlobalBounds().contains(static_cast<Vector2f>(Mouse::getPosition(window)))) {
            isMenuOpen = false;
        }
        if (event.mouseButton.button == Mouse::Right || isErasing == true) {
            isErasing = true;
            handleDrawing(event);
        }
        handleBrushSizeClick(event);
    }
}

void handleMouseMovedEvent(const Event& event) {
    if (isDrawing && !isShiftKeyPressed) {
        handleDrawing(event);
    }

    if (isMousedown && isShiftKeyPressed) {
        lineEndPreview = Vector2f(event.mouseMove.x, event.mouseMove.y);
    }
}

void handleMouseButtonReleasedEvent(const Event& event) {
    isDrawing = false;
    isErasing = false;
    isMousedown = false;

    if (isShiftKeyPressed) {
        Vector2i mousePos = Mouse::getPosition(window);
        lineEnd.x = std::round((mousePos.x - 20 - 20) / scale) * scale + 20 + 20;
        lineEnd.y = std::round((mousePos.y - 20) / scale) * scale + 20;
        
        drawShiftLines(event);
        pixel.setPosition(Vector2f(-scale, -scale));
        pixel.setFillColor(Color::Transparent);
        isShiftKeyPressed = false;
        
    } else {
        handleSaveButtonClick(event);
    }
}

void handleBrushSizeClick(const Event& event) {
    if (event.mouseButton.button == Mouse::Left) {
        Vector2i mousePos = Mouse::getPosition(window);
        if (brushSizeSmall.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
            isDrawing = false;
            brushSizeValue = brushSizeValueStart;
            updateBrush();
        } else if (brushSizeMed.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
            isDrawing = false;
            brushSizeValue = brushSizeValueMax;
            updateBrush();
        } else if (brushSizeMax.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
            isDrawing = false;
            brushSizeValue = brushSizeValueMax;
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
    int x = (mousePos.x - 20 - 20) / scale;
    int y = ((mousePos.y - 20) / scale) - scale;

    int x_nearest = (x + brushSizeValue / 2) / scale * scale;
    int y_nearest = (y + brushSizeValue / 2) / scale * scale;

    if (x >= 0 && x < sizeX && y >= 0 && y < sizeY) {
        for (int i = x_nearest - brushSizeValue; i <= x_nearest + brushSizeValue; i += scale) {
            for (int j = y_nearest - brushSizeValue; j <= y_nearest + brushSizeValue; j += scale) {
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

void drawShiftLines(const Event& event) {
    if (isShiftKeyPressed) {
        isDrawing = false;

        int startPixelX = (static_cast<int>((lineStart.x) / scale))-(scale-(scale/4));
        int startPixelY = (static_cast<int>((lineStart.y) / scale))-(scale+(scale/4));
        int endPixelX = (static_cast<int>((lineEnd.x) / scale))-(scale-(scale/4));
        int endPixelY = (static_cast<int>((lineEnd.y) / scale))-(scale+(scale/4));

        if (startPixelX != endPixelX || startPixelY != endPixelY) {
            float deltaX = static_cast<float>(endPixelX - startPixelX);
            float deltaY = static_cast<float>(endPixelY - startPixelY);
            float steps = std::max(std::abs(deltaX), std::abs(deltaY));

            for (float t = 0.0f; t <= 1.0f; t += 1.0f / steps) {
                int x = static_cast<int>(startPixelX + t * deltaX);
                int y = static_cast<int>(startPixelY + t * deltaY);

                int x_nearest = (x + brushSizeValue / 2) / scale * scale;
                int y_nearest = (y + brushSizeValue / 2) / scale * scale;

                if (x >= 0 && x < sizeX && y >= 0 && y < sizeY) {
                    for (int i = x_nearest - brushSizeValue; i <= x_nearest + brushSizeValue; i += scale) {
                        for (int j = y_nearest - brushSizeValue; j <= y_nearest + brushSizeValue; j += scale) {
                            if (i >= 0 && i < sizeX && j >= 0 && j < sizeY) {
                                if (isErasing) {
                                    image.setPixel(i, j, Color::Transparent);
                                } else {
                                    image.setPixel(i, j, penColor);
                                }

                
                            }
                        }
                    }
                }
texture.loadFromImage(image);

            }
        }
        

        lineStart = Vector2f(-scale, -scale);
        lineEnd = Vector2f(-scale, -scale);
    }
}



void updateBrush() {
    brush.setSize(Vector2f(scale, scale));
    brush.setOrigin(brush.getSize());
}

void render() {
    renderTexture.clear(Color(85, 107, 47));
    window.clear(Color(85, 107, 47));
    window.draw(menuBar);
    window.draw(menuFile);
    window.draw(topRulerPattern);
    window.draw(leftRulerPattern);

    drawGridLines(window);
    window.draw(topLine);
    window.draw(leftLine);
    sprite.setPosition((scale * 6), (scale * 11)); // Grid är nu från precis ovan penseln och slutar 1px under.
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
    int x_nearest = (static_cast<int>((mousePos.x - 20 - 20) / scale) * scale) + 20 + 20 + (scale / 1);
    int y_nearest = (static_cast<int>((mousePos.y - 20) / scale) * scale) + 20 + (scale / 2);

    brush.setPosition(static_cast<float>(x_nearest), static_cast<float>(y_nearest));
    brush.setFillColor(Color(150, 150, 150, 200));
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

    RectangleShape pixel(Vector2f(scale, scale));
    if (isShiftKeyPressed) {
        int startPixelX = static_cast<int>(lineStartPreview.x / scale);
        int startPixelY = static_cast<int>(lineStartPreview.y / scale);
        int endPixelX = static_cast<int>(lineEndPreview.x / scale);
        int endPixelY = static_cast<int>(lineEndPreview.y / scale);

        if (startPixelX != endPixelX || startPixelY != endPixelY) {
            float deltaX = static_cast<float>(endPixelX - startPixelX);
            float deltaY = static_cast<float>(endPixelY - startPixelY);
            float steps = std::max(std::abs(deltaX), std::abs(deltaY)) * scale;

            for (float t = 0.0f; t <= 1.0f; t += scale / steps) {
                int x = static_cast<int>(startPixelX + t * deltaX);
                int y = static_cast<int>(startPixelY + t * deltaY);

                int x_nearest = (static_cast<int>(x));
                int y_nearest = (static_cast<int>(y));

                if (x >= 0 && x < initialSizeX && y >= 0 && y < initialSizeY) {

                    pixel.setPosition(Vector2f(x_nearest*scale, y_nearest*scale));
                    pixel.setFillColor(Color(penColor));
                    window.draw(pixel);
                }
            }
        }
    }
    pixel.setPosition(Vector2f(-scale, -scale)); 
    pixel.setFillColor(Color::Transparent);

    window.display();
}

};

int main() {

    try {
        DrawingApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Fel: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
