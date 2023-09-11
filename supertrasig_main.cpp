#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <thread>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <filesystem>
#include <cmath>

using namespace sf;
namespace fs = std::__fs::filesystem;

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
    int baseSizeX = 128;
    int baseSizeY = 64;
    int initialSizeX = (baseSizeX + static_cast<int>(scale * 2.5)) * scale;
    int initialSizeY = (baseSizeY + static_cast<int>(scale * 2.5)) * scale;
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
    std::stack<sf::Image> imageHistory;
    std::stack<sf::Image> redoHistory;

    RectangleShape infoSpace;
    Text infoSymbolErase;
    RectangleShape brushSize;
    RectangleShape brushSizeSmall;
    RectangleShape brushSizeMed;
    RectangleShape brushSizeMax;

    Color penColor = Color::Black;

    float brushSizeValueStart = 1.0;
    float brushSizeValueMed = 2.0;
    float brushSizeValueMax = 4.0;
    float t;
    RectangleShape brush;
    float brushSizeValue;
    float selectedBrushSize;
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
        Color bgColor(85, 107, 47, 150);
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
        selectedBrushSize = brushSizeValueStart;
        updateBrush();

        isMenuOpen = false;
        isShiftKeyPressed = false;

        isDrawing = false;
        isErasing = false;
        lineStart = Vector2f(-scale, -scale);
        lineEnd = Vector2f(-scale, -scale);
        lineStartPreview = Vector2f(-scale, -scale);
        lineEndPreview = Vector2f(-scale, -scale);

        sf::RenderWindow window(sf::VideoMode(800, 600), "File Open Dialog");

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

bool openAndLoadImage(sf::Image& image) {
    sf::RenderWindow fileDialog(sf::VideoMode(400, 300), "File Open Dialog");
    fileDialog.setPosition(sf::Vector2i(100, 100));

    sf::Font font;
    font.loadFromFile("/System/Library/Fonts/Supplemental/Courier New.ttf");

    std::string folderPath = "./save";
    std::vector<std::string> fileList;  
    int selectedFileIndex = 0;  

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.length() >= 4 && filename.substr(filename.length() - 4) == ".bmp") {
                fileList.push_back(filename);
            }
        }
    }

    while (fileDialog.isOpen()) {
        sf::Event event;
        while (fileDialog.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                fileDialog.close();
                return false;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    fileDialog.close();
                    return false;
                }
                else if (event.key.code == sf::Keyboard::Up) {
                    if (selectedFileIndex > 0) {
                        selectedFileIndex--;
                    }
                }
                else if (event.key.code == sf::Keyboard::Down) {
                    if (selectedFileIndex < fileList.size() - 1) {
                        selectedFileIndex++;
                    }
                }
                else if (event.key.code == sf::Keyboard::Enter) {
    std::string selectedFile = folderPath + "/" + fileList[selectedFileIndex];
    sf::Image loadedImage;
    if (loadedImage.loadFromFile(selectedFile)) {
        texture.loadFromImage(loadedImage);
        window.display();
    }
    fileDialog.close();
    return true; 
}
            }
        }

        fileDialog.clear(sf::Color::White);
        
        int yOffset = 40;
        for (size_t i = 0; i < fileList.size(); ++i) {
            sf::Text fileText(fileList[i], font, 16);
            fileText.setFillColor(sf::Color::Black);
            fileText.setPosition(10, yOffset);
            if (i == selectedFileIndex) {
                
                fileText.setFillColor(sf::Color::Red);
            }
            fileDialog.draw(fileText);
            yOffset += 20;
        }

        fileDialog.display();
    }
    return false;
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
        } while (std::__fs::filesystem::exists(filePath));

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

void saveImageToHistory() {
    imageHistory.push(image);
    while (!redoHistory.empty()) {
        redoHistory.pop();
    }
}

void undo() {
    if (!imageHistory.empty()) {
        redoHistory.push(imageHistory.top()); 
        imageHistory.pop(); 
        if (!imageHistory.empty()) {
            image = imageHistory.top(); 
            texture.loadFromImage(image);
        }
    }
}

void redo() {
    if (!redoHistory.empty()) {
        imageHistory.push(image); 
        image = redoHistory.top(); 
        redoHistory.pop(); 
        texture.loadFromImage(image);
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
     /*    isShiftKeyPressed = true;
        Vector2i mousePos = Mouse::getPosition(window);
        Vector2f spritePosition = sprite.getPosition();
        Vector2f mapMousePos = window.mapPixelToCoords(mousePos) - spritePosition;
        lineStartPreview = Vector2f(mapMousePos.x, mapMousePos.y);
        lineEndPreview = lineStartPreview;
        if (isDrawing) {
            lineStart.x = std::round(mapMousePos.x);
            lineStart.y = std::round(mapMousePos.y);
            lineEnd = lineStart;
        } */
    }
    if (event.key.code == Keyboard::X) {
    isErasing = true;
    }
    if (event.key.code == Keyboard::Num1) {
        selectedBrushSize = brushSizeValueStart;
        updateBrush();
    }
    if (event.key.code == Keyboard::Num2) {
        selectedBrushSize = brushSizeValueMed;
        updateBrush();
    }
    if (event.key.code == Keyboard::Num3) {
        selectedBrushSize = brushSizeValueMax;
        updateBrush();
    }
    if (event.key.code == Keyboard::Z && (event.key.control || event.key.system) && !(Keyboard::isKeyPressed(Keyboard::LShift) || Keyboard::isKeyPressed(Keyboard::RShift))) {
        undo();
    }
    if (event.key.code == Keyboard::Z && (event.key.control || event.key.system) && Keyboard::isKeyPressed(Keyboard::LShift)) {
        redo();
    }
    if (event.key.code == sf::Keyboard::O || event.key.code == sf::Keyboard::L) {
        if (openAndLoadImage(image)) {
                            sf::Texture texture;
                            texture.loadFromImage(image);
                            sf::Sprite sprite(texture);
        }
    }
    if (event.key.code == Keyboard::S) {
        saveImageAsCArray();
        saveImageAsBMP();
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
        isDrawing = true;
        if (event.mouseButton.button == Mouse::Left) {
            Vector2i mousePos = Mouse::getPosition(window);
      //      Vector2f spritePosition = sprite.getPosition();
        //    Vector2f mapMousePos = window.mapPixelToCoords(mousePos) - spritePosition;
        //    lineStartPreview = Vector2f(mapMousePos.x, mapMousePos.y);
            lineEndPreview = lineStartPreview;
            if (isShiftKeyPressed) {
                isDrawing = false;
          //      lineStart = Vector2f(mapMousePos.x, mapMousePos.y);
          //      lineStartPreview = Vector2f(mapMousePos.x, mapMousePos.y);
                lineEndPreview = lineStartPreview;
                lineEnd = lineStart;
            } else if (menuFile.getGlobalBounds().contains(static_cast<Vector2f>(Mouse::getPosition(window)))) {
                isMenuOpen = !isMenuOpen;
            }
            if (!isShiftKeyPressed) {
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
        Vector2i mousePos = Mouse::getPosition(window);
     //   Vector2f spritePosition = sprite.getPosition();
       // Vector2f mapMousePos = window.mapPixelToCoords(mousePos) - spritePosition;
       // lineEndPreview = Vector2f(mapMousePos.x, mapMousePos.y);
    }
}

void handleMouseButtonReleasedEvent(const Event& event) {
    isDrawing = false;
    isErasing = false;
    isMousedown = false;

    if (isShiftKeyPressed) {
        Vector2i mousePos = Mouse::getPosition(window);
      //  Vector2f spritePosition = sprite.getPosition();
       // lineEnd = Vector2f(mousePos.x - spritePosition.x, mousePos.y - spritePosition.y);

       // Vector2f mapMousePos = window.mapPixelToCoords(mousePos) - spritePosition;
        //lineEnd = Vector2f(mapMousePos.x, mapMousePos.y);
        
        drawShiftLines(event);
        pixel.setPosition(Vector2f(-scale, -scale));
        pixel.setFillColor(Color::Transparent);
    } else {
        handleSaveButtonClick(event);
    }
}

void handleBrushSizeClick(const Event& event) {
    if (event.mouseButton.button == Mouse::Left) {
        Vector2i mousePos = Mouse::getPosition(window);
        if (brushSizeSmall.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
            isDrawing = false;
            selectedBrushSize = brushSizeValueStart;
            updateBrush();
        } else if (brushSizeMed.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
            isDrawing = false;
            selectedBrushSize = brushSizeValueMed;
            updateBrush();
        } else if (brushSizeMax.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
            isDrawing = false;
            selectedBrushSize = brushSizeValueMax;
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

    isDrawing = true;
/*     Vector2i mousePos = Mouse::getPosition(window);
    Vector2f spritePosition = sprite.getPosition();
    Vector2f mapMousePos = window.mapPixelToCoords(mousePos) - spritePosition;
    int x = ((mapMousePos.x)/scale);
    int y = ((mapMousePos.y)/scale); */
    Vector2i mousePos = Mouse::getPosition(window);
    int scaleAdj = scale/selectedBrushSize;
    int x = (mousePos.x - 20 - 20) / scaleAdj;
    int y = ((mousePos.y - 20) / scaleAdj) - scaleAdj;

    int x_nearest = (x / scale) * scale;
    int y_nearest = (y / scale) * scale;

    if (selectedBrushSize > brushSizeValueStart) {
        x_nearest -= (selectedBrushSize / 2);
        y_nearest -= (selectedBrushSize / 2);
    }
    // Om pendeln går utanför så får den inte börja i andra änden! + nu går den en pixel för lågt.
   // if (mapMousePos.x >= 0 && mapMousePos.x < sizeX && mapMousePos.y >= 0 && mapMousePos.y < sizeY) {
        for (int i = 0; i < selectedBrushSize; i++) {
            for (int j = 0; j < selectedBrushSize; j++) {
                                    if (isErasing) {
                        image.setPixel(x_nearest + i, y_nearest + j, Color::Transparent);
                    } else {
                        image.setPixel(x_nearest + i, y_nearest + j, penColor);
                    }
            }
        }
   // }
    saveImageToHistory();
    texture.loadFromImage(image);
}    

// Försök få shiftlines att bete sig som previewlines i render
void drawShiftLines(const Event& event) {
    if (isShiftKeyPressed) {
        isDrawing = false;
        brushSizeValue = scale*selectedBrushSize;
        Vector2i mousePos = Mouse::getPosition(window);
      //  Vector2f spritePosition = sprite.getPosition();
      //  Vector2f mapMousePos = window.mapPixelToCoords(mousePos) - spritePosition;

        int startPixelX = static_cast<int>(lineStart.x / scale);
        int startPixelY = static_cast<int>(lineStart.y / scale);
        int endPixelX = static_cast<int>(lineEnd.x / scale);
        int endPixelY = static_cast<int>(lineEnd.y / scale);

        if (startPixelX != endPixelX || startPixelY != endPixelY) {
            float deltaX = static_cast<float>(endPixelX - startPixelX);
            float deltaY = static_cast<float>(endPixelY - startPixelY);
            float steps = std::max(std::abs(deltaX) / selectedBrushSize, std::abs(deltaY) / selectedBrushSize);

            for (float t = 0.0f; t <= 1.0f; t += 1.0f / steps) {
                int x = static_cast<int>(startPixelX + t * deltaX);
                int y = static_cast<int>(startPixelY + t * deltaY);

                int x_nearest = ((x + (selectedBrushSize)) / scale * scale);
                int y_nearest = ((y + (selectedBrushSize )) / scale * scale);

                if (x >= 0 && x < sizeX && y >= 0 && y < sizeY) {
                    for (int i = x_nearest - static_cast<int>(selectedBrushSize / scale); i <= x_nearest + static_cast<int>(selectedBrushSize / scale); i++) {
                        for (int j = y_nearest - static_cast<int>(selectedBrushSize / scale); j <= y_nearest + static_cast<int>(selectedBrushSize / scale); j++) {
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
                saveImageToHistory();
                texture.loadFromImage(image);
            }
        }
        lineStart = Vector2f(-scale, -scale);
        lineEnd = Vector2f(-scale, -scale);
    }
}

void updateBrush() {
    brush.setSize(Vector2f(scale*selectedBrushSize, scale*selectedBrushSize));
    brush.setOrigin(brush.getSize().x, brush.getSize().y);
}

void render() {
    renderTexture.clear(Color(bgColor));
    window.clear(Color(85, 107, 47));
    window.draw(menuBar);
    window.draw(menuFile);
    window.draw(topRulerPattern);
    window.draw(leftRulerPattern);

    window.draw(topLine);
    window.draw(leftLine);
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

    drawGridLines(window);

    window.draw(infoSpace);
    window.draw(brushSize);
    window.draw(brushSizeSmall);
    window.draw(brushSizeMed);
    window.draw(brushSizeMax);

    if (isErasing) {
        window.draw(infoSymbolErase);
    }

    sprite.setPosition((scale * 6), (scale * 11)); 
    window.draw(sprite);

    Vector2i mousePos = Mouse::getPosition(window);
 //   Vector2f spritePosition = sprite.getPosition();
  //  FloatRect spriteBounds = sprite.getGlobalBounds();
    brushSizeValue = scale*selectedBrushSize;
    int scaleSq = scale;
    int x_nearest = static_cast<int>(static_cast<float>((mousePos.x / scaleSq) * scaleSq));
    int y_nearest = static_cast<int>(static_cast<float>((mousePos.y / scaleSq) * scaleSq));
    int brushSizeHalf = std::max(static_cast<float>(brushSizeValue/2), brushSizeValueStart*scale);

//if (mousePos.x >= spritePosition.x && mousePos.x <= spritePosition.x + spriteBounds.width &&
 //   mousePos.y >= spritePosition.y && mousePos.y <= spritePosition.y + spriteBounds.height) {
        brush.setPosition(static_cast<int>(x_nearest) + brushSizeHalf, static_cast<int>(y_nearest) + brushSizeHalf);
        brush.setFillColor(Color(150, 150, 150, 200));
        window.draw(brush);
   // }

    RectangleShape pixel(Vector2f(brushSizeValue, brushSizeValue));
  /*   if (isShiftKeyPressed) {
        Vector2f spritePosition = sprite.getPosition();
        FloatRect spriteBounds = sprite.getGlobalBounds();
        Vector2f adjLineStartPreview;
        adjLineStartPreview.x = static_cast<float>(lineStartPreview.x + spritePosition.x);
        adjLineStartPreview.y = static_cast<float>(lineStartPreview.y + spritePosition.y);
        Vector2f adjLineEndPreview;
        adjLineEndPreview.x = static_cast<float>(lineEndPreview.x + spritePosition.x);
        adjLineEndPreview.y = static_cast<float>(lineEndPreview.y + spritePosition.y);


        int startPixelX = static_cast<int>(adjLineStartPreview.x);
        int startPixelY = static_cast<int>(adjLineStartPreview.y);
        int endPixelX = static_cast<int>(adjLineEndPreview.x);
        int endPixelY = static_cast<int>(adjLineEndPreview.y);

        if (startPixelX != endPixelX || startPixelY != endPixelY) {
            float deltaX = static_cast<float>(endPixelX - startPixelX);
            float deltaY = static_cast<float>(endPixelY - startPixelY);
            float steps = std::max(std::abs(deltaX) / brushSizeValue, std::abs(deltaY) / brushSizeValue);

            for (float t = 0.0f; t <= 1.0f; t += 1.0f / steps) {
                int x = static_cast<int>(startPixelX + t * deltaX);
                int y = static_cast<int>(startPixelY + t * deltaY);

                int x_nearest = static_cast<int>((x / scale) * scale);
                int y_nearest = static_cast<int>((y / scale) * scale);

                if (x >= 0 && x < sizeX*scale && y >= 0 && y < sizeY*scale) {
                    for (int i = x_nearest - static_cast<int>(selectedBrushSize / scale); i <= x_nearest + static_cast<int>(selectedBrushSize / scale); i++) {
                        for (int j = y_nearest - static_cast<int>(selectedBrushSize / scale); j <= y_nearest + static_cast<int>(selectedBrushSize / scale); j++) {
                           
                            pixel.setPosition(Vector2f(i, j));
                            pixel.setFillColor(Color(penColor));
                            window.draw(pixel);
                        }
                    }
                }
            }
        }
    } */
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
