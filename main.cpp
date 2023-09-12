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
    DrawingApp() : window(VideoMode(initialSizeX, initialSizeY), "Pixel") {
    initialize();
    }

    void run() {
        while (window.isOpen()) {
            Event event;
            handleEvents();
            Time elapsedTime = frameClock.getElapsedTime();
            if (elapsedTime.asSeconds() >= frameTime) {
                render();
                frameClock.restart();
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
    const float scale = 8.0f;
    const int baseSizeX = 128;
    const int baseSizeY = 64;

    RectangleShape squareShapes[64];
    RectangleShape squareShapesTop[130]; // + 2 för extra ruta vid nollan

    Color bgColor = Color(85, 107, 47);
    Color elementColor = Color(60, 60, 60);
    Color elementColorAlfa = Color(60, 60, 60, 150);
    Color mutedWhite = Color(150, 150, 150, 200);

    Color gridColor2 = Color(90, 110, 70);
    Color gridColor = Color(80, 100, 40, 150);

    Color penColor = Color::Black;

    float brushSizeValueSmall = 1.0;
    float brushSizeValueMed = 2.0;
    float brushSizeValueMax = 3.0;

    float lineWidth = 1.0;
    int sizeX = (baseSizeX * static_cast<int>(scale));
    int sizeY = (baseSizeY * static_cast<int>(scale));
    int miniWidth = static_cast<int>(scale);
    int smallWidth = static_cast<int>(scale * 3);
    int medWidth = static_cast<int>(scale * 6);

    float extraTopHeight = smallWidth + medWidth + lineWidth;
    float extraBottomHeight = medWidth;
    float extraWidth = medWidth + smallWidth + baseSizeX + smallWidth;

    int initialSizeX = sizeX + extraWidth;
    int initialSizeY = sizeY + extraTopHeight + extraBottomHeight;

    RectangleShape menuBar;
    Text menuFile;
    Font font;
    Vector2f menuSize = Vector2f(initialSizeX, smallWidth);
    float rulerSize = medWidth;
    float textSpacing = scale;
    RectangleShape dropDown;
    Text menuSaveText;
    Text menuLoadText;
    bool isMenuOpen = false;

    bool isSaveButtonClicked;
    Clock saveButtonClock;

    RenderWindow window;

    Image image;
    Texture texture;
    Sprite sprite;
    Sprite previewSprite;
    Cursor cursor;

    std::stack<Image> imageHistory;
    std::stack<Image> redoHistory;

    RectangleShape topRuler;
    RectangleShape topLine;
    RectangleShape leftRuler;
    RectangleShape leftLine;
    Text leftRulerText;
    Text topRulerText;
    std::vector<Text> leftRulerTexts;
    std::vector<Text> topRulerTexts;
    const float squareSize = scale;
    int zeroBase = baseSizeX;

    int numSquares = std::extent<decltype(squareShapes)>::value;
    int numSquaresTop = std::extent<decltype(squareShapesTop)>::value;

    Vector2f infoSpacePos = Vector2f(static_cast<float>(window.getSize().x) - baseSizeX - smallWidth, extraTopHeight + baseSizeY + (medWidth * 2));
    RectangleShape infoSpace;
    RectangleShape previewScreen;
    bool outlineColorOnOff = false;
    RectangleShape checkbox;
    Text checkboxInfoText;
    Text checkmark;

    Text infoSymbolErase;

    bool isDrawing = false;
    bool isErasing = false;

    float brushSizeValueStart = brushSizeValueSmall;
    float selectedBrushSize = brushSizeValueStart;    

    RectangleShape brushSizeSmall;
    RectangleShape brushSizeMed;
    RectangleShape brushSizeMax;

    float brushSizeValue = selectedBrushSize;
    RectangleShape brushSize;
    RectangleShape brush;

    bool isShiftKeyPressed = false;
    Vector2f lineStartPreview;
    Vector2f lineEndPreview;
    Vector2f lineStart;
    Vector2f lineEnd;
    RectangleShape preview;

    bool isMousedown = false;

    Clock frameClock;
    const float frameTime = 0.1f;
/* 
    std::vector<RectangleShape> squareShapes; Jag hade velat ha dessa flexibla!
    std::vector<RectangleShape> squareShapesTop;  
    
    int numSquares = sizeof(squareShapes) / sizeof(squareShapes[0]);
    int numSquaresTop = sizeof(squareShapesTop) / sizeof(squareShapesTop[0]);
    */

void initialize() {

    create_menuBar();
        if (!font.loadFromFile("/System/Library/Fonts/Supplemental/Courier New.ttf")) {
            throw std::runtime_error("Unable to load font");
        }
        createRulers();
        createRulerText();
        createTopSquareShapes();
        createLeftSquareShapes();

        create_infoSpace();
        create_infoSymbols();
        create_infoBrushes();

    updateBrush();

    lineStart = Vector2f(-scale, -scale);
    lineEnd = Vector2f(-scale, -scale);
    lineStartPreview = Vector2f(-scale, -scale);
    lineEndPreview = Vector2f(-scale, -scale);

    RenderWindow window(VideoMode(800, 600), "File Open Dialog");

    image.create((baseSizeX), (baseSizeY), Color::Transparent);
        texture.loadFromImage(image);
        sprite.setTexture(texture, true);
        sprite.setScale(scale, scale);
             create_preview();
        previewScreen.setTexture(&texture);

    frameClock.restart();
/*
     Det här hade varit bättre sätt att ange storleken i initialize
    RectangleShape squareShapes[baseSizeY];
    RectangleShape squareShapesTop[baseSizeX];
*/    
}

// ### - Nu bygger vi alla element för fönstret - ###
void create_menuBar() {
    menuBar.setSize(menuSize);
    menuBar.setFillColor(Color::White);
    menuBar.setPosition(0, 0);

    create_dropDown();
    create_menuText();
}

void create_menuText() {
    menuFile.setFont(font);
    menuFile.setCharacterSize(12);
    menuFile.setString("File");
    menuFile.setPosition(menuSize.y, menuSize.y - textSpacing - menuFile.getCharacterSize());
    menuFile.setFillColor(Color::Black);
}

void create_dropDown() {
    int menuItems = 2;
// Det här borde göras mycket snyggare, framförallt avstånden
    dropDown.setSize(Vector2f(medWidth + smallWidth, ((smallWidth + textSpacing) * menuItems) - textSpacing));
    dropDown.setFillColor(Color::White);
    dropDown.setPosition(smallWidth - miniWidth, smallWidth);

    menuSaveText.setFont(font);
    menuSaveText.setCharacterSize(12);
    menuSaveText.setFillColor(Color::Black);
    menuSaveText.setString("Save");
    menuSaveText.setPosition(menuSize.y, smallWidth + textSpacing);

    menuLoadText.setFont(font);
    menuLoadText.setCharacterSize(12);
    menuLoadText.setFillColor(Color::Black);
    menuLoadText.setString("Load");
    menuLoadText.setPosition(menuSize.y, (smallWidth * 2) + textSpacing);
}

void create_GridLines(RenderWindow& window) {
    int positionLeftCornerSpriteX = sprite.getPosition().x;
    int positionRightCornerSpriteX = sprite.getPosition().x + sizeX;
    int positionSpriteY = sprite.getPosition().y + lineWidth;
    int positionTopCornerSpriteY = sprite.getPosition().y;
    int positionBottomCornerSpriteY = sprite.getPosition().y + sizeY;

    for (int x = positionLeftCornerSpriteX + scale; x <= positionRightCornerSpriteX; x += scale) { // justerar en ruta åt höger
        VertexArray line2(Lines, 2);
        line2[0].position = Vector2f(static_cast<float>(x), static_cast<float>(positionSpriteY));
        line2[1].position = Vector2f(static_cast<float>(x), static_cast<float>(positionSpriteY + sizeY));
        line2[0].color = gridColor;
        line2[1].color = gridColor;
        window.draw(line2);
    }
    for (int y = positionTopCornerSpriteY; y <= positionBottomCornerSpriteY; y += scale) {
        VertexArray line2(Lines, 2);
        line2[0].position = Vector2f(static_cast<float>(positionLeftCornerSpriteX), static_cast<float>(y));
        line2[1].position = Vector2f(static_cast<float>(positionRightCornerSpriteX), static_cast<float>(y));
        line2[0].color = gridColor;
        line2[1].color = gridColor;
        window.draw(line2);
    }

    for (int x = positionLeftCornerSpriteX + (scale * 8); x <= positionRightCornerSpriteX; x += scale * 8) {
        VertexArray line(Lines, 2);
        line[0].position = Vector2f(static_cast<float>(x), static_cast<float>(positionSpriteY));
        line[1].position = Vector2f(static_cast<float>(x), static_cast<float>(positionSpriteY + sizeY));
        line[0].color = gridColor2;
        line[1].color = gridColor2;
        window.draw(line);
    }
    for (int y = positionTopCornerSpriteY + (scale * 8); y <= positionBottomCornerSpriteY; y += scale * 8) {
        VertexArray line(Lines, 2);
        line[0].position = Vector2f(static_cast<float>(positionLeftCornerSpriteX), static_cast<float>(y));
        line[1].position = Vector2f(static_cast<float>(positionRightCornerSpriteX), static_cast<float>(y));
        line[0].color = gridColor2;
        line[1].color = gridColor2;
        window.draw(line);
    }
}

void createRulers() {
    topRuler.setSize(Vector2f(sizeX, rulerSize));
    topRuler.setFillColor(Color::Transparent);
    topRuler.setPosition(rulerSize, menuSize.y);

    topLine.setSize(Vector2f(sizeX, lineWidth));
    topLine.setFillColor(elementColor);
    topLine.setPosition(rulerSize, extraTopHeight - lineWidth);

    leftRuler.setSize(Vector2f(rulerSize, sizeY));
    leftRuler.setFillColor(Color::Transparent);
    leftRuler.setPosition(0, extraTopHeight);

    leftLine.setSize(Vector2f(lineWidth, sizeY));
    leftLine.setFillColor(elementColor);
    leftLine.setPosition(rulerSize, extraTopHeight);
}

void createTopSquareShapes() { // En extra ruta till 0:an
    for (int i = 0; i < numSquaresTop; ++i) {
        RectangleShape square;
        square.setSize(Vector2f(squareSize, squareSize));
        if ((i - baseSizeX) % 2 == 0) {
            square.setFillColor(Color::Transparent);
        } else {
            square.setFillColor(elementColor);
        }
        square.setPosition(rulerSize + (i * squareSize) - (squareSize * 2), extraTopHeight - squareSize - lineWidth);
        squareShapesTop[i] = square;
    } 
}

void createLeftSquareShapes() {
    for (int i = 0; i < numSquares; ++i) {
        RectangleShape square;
        square.setSize(Vector2f(squareSize, squareSize));
        if (i % 2 == 0) {
            square.setFillColor(Color::Transparent);
        } else {
            square.setFillColor(elementColor);
        }
        square.setPosition(rulerSize - squareSize, extraTopHeight + (i * squareSize));
        squareShapes[i] = square;
    }
}

void createRulerText() {
    topRulerText.setFont(font);
    topRulerText.setCharacterSize(12);
    topRulerText.setFillColor(Color::Black);

    leftRulerText.setFont(font);
    leftRulerText.setCharacterSize(12);
    leftRulerText.setFillColor(Color::Black);

    for (int i = 8; i <= baseSizeX; i += 8) {
        Text rulerText(std::to_string(i), font, 12);
        rulerText.setFillColor(Color::Black);
        if (i <= 9) { // vänsterkantens linjalbredd + värdet * rutstorlek - centrera över rutan - första rutan 1
            rulerText.setPosition(rulerSize + (i * squareSize) - squareSize, rulerSize - lineWidth);
        } else if (i >= 10 && i <= 99) {
            rulerText.setPosition(rulerSize + (i * squareSize) - (squareSize/2) - squareSize, rulerSize - lineWidth);
        } else {
            rulerText.setPosition(rulerSize + (i * squareSize) - squareSize - squareSize, rulerSize - lineWidth);
        }
        topRulerTexts.push_back(rulerText);
    }
    for (int i = 8; i <= baseSizeY; i += 8) {
        Text rulerText(std::to_string(i), font, 12);
        rulerText.setFillColor(Color::Black);
        if (i <= 9) { // 6 är fontstorleken/2
        rulerText.setPosition(rulerSize / 2 + 6, extraTopHeight - 12 + (i * squareSize));
        } else {
        rulerText.setPosition(rulerSize / 2, extraTopHeight - 12 + (i * squareSize));
        }
        leftRulerTexts.push_back(rulerText);
    }
}

void create_preview() {
    previewScreen.setSize(Vector2f(baseSizeX, baseSizeY));
    previewScreen.setPosition(static_cast<float>(window.getSize().x) - baseSizeX - smallWidth, extraTopHeight);
    previewScreen.setOutlineThickness(2);
    if (outlineColorOnOff) {
        previewScreen.setOutlineColor(elementColor);
    } else {
        previewScreen.setOutlineColor(bgColor);
    };
    texture.loadFromImage(image);
    previewSprite.setTexture(texture);
    previewSprite.setPosition(previewScreen.getPosition());
    previewSprite.setScale(baseSizeX, baseSizeY);

    checkbox.setSize(Vector2f(smallWidth/2, smallWidth/2));
    checkbox.setPosition(previewScreen.getPosition().x, previewScreen.getPosition().y + baseSizeY + smallWidth);
    checkbox.setFillColor(Color(mutedWhite));
    checkbox.setOutlineThickness(1);
    checkbox.setOutlineColor(Color(elementColor));

    checkmark.setPosition(Vector2f(checkbox.getPosition().x, checkbox.getPosition().y - (checkbox.getSize().y / 2) + 1));
    checkmark.setFont(font);
    checkmark.setCharacterSize(16);
    checkmark.setStyle(Text::Bold);
    checkmark.setFillColor(Color::Black);
    checkmark.setString("X");

    checkboxInfoText.setFont(font);
    checkboxInfoText.setCharacterSize(12);
    checkboxInfoText.setFillColor(Color::Black);
    checkboxInfoText.setString("Border on/off");
    checkboxInfoText.setPosition(checkbox.getPosition().x + (checkbox.getSize().x * 2), checkbox.getPosition().y);
}

void create_infoSpace() {
    infoSpace.setSize(Vector2f(baseSizeX, sizeY - baseSizeY - medWidth));
    infoSpace.setPosition(infoSpacePos);
    infoSpace.setOutlineThickness(2);
    infoSpace.setOutlineColor(bgColor);
    infoSpace.setFillColor(Color::Transparent);
}

void create_infoSymbols() {
    infoSymbolErase.setFont(font);
    infoSymbolErase.setCharacterSize(16);
    infoSymbolErase.setStyle(Text::Bold);
    infoSymbolErase.setFillColor(Color::Black);
    infoSymbolErase.setString("X");
    infoSymbolErase.setPosition(infoSpace.getPosition().x, infoSpace.getPosition().y);
}

void create_infoBrushes() {
    brushSize.setPosition(infoSpace.getPosition().x, infoSymbolErase.getPosition().y + smallWidth);
    brushSize.setFillColor(Color::Transparent);

    brushSizeSmall.setSize(Vector2f(brushSizeValueSmall * scale, brushSizeValueSmall * scale));
    brushSizeSmall.setPosition(brushSize.getPosition().x, brushSize.getPosition().y + miniWidth);
    brushSizeSmall.setFillColor(Color::Black);

    brushSizeMed.setSize(Vector2f(brushSizeValueMed * scale, brushSizeValueMed * scale));
    brushSizeMed.setPosition(brushSize.getPosition().x, brushSizeSmall.getPosition().y + brushSizeSmall.getSize().y + miniWidth);
    brushSizeMed.setFillColor(Color::Black);

    brushSizeMax.setSize(Vector2f(brushSizeValueMax * scale, brushSizeValueMax * scale));
    brushSizeMax.setPosition(brushSize.getPosition().x, brushSizeMed.getPosition().y + brushSizeMed.getSize().y + miniWidth);
    brushSizeMax.setFillColor(Color::Black);
}

void updateBrush() {
    brush.setSize(Vector2f(selectedBrushSize, selectedBrushSize));
    brush.setOrigin(selectedBrushSize, selectedBrushSize);
}
// Nu har vi skapat alla element

// ### - Save och load, history och undo - ###
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

bool openAndLoadImage(Image& image) {
    RenderWindow fileDialog(VideoMode(400, 300), "File Open Dialog");
    fileDialog.setPosition(Vector2i(100, 100));

    Font font;
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
        Event event;
        while (fileDialog.pollEvent(event)) {
            if (event.type == Event::Closed) {
                fileDialog.close();
                return false;
            }
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Escape) {
                    fileDialog.close();
                    return false;
                }
                else if (event.key.code == Keyboard::Up) {
                    if (selectedFileIndex > 0) {
                        selectedFileIndex--;
                    }
                }
                else if (event.key.code == Keyboard::Down) {
                    if (selectedFileIndex < fileList.size() - 1) {
                        selectedFileIndex++;
                    }
                }
                else if (event.key.code == Keyboard::Enter) {
                std::string selectedFile = folderPath + "/" + fileList[selectedFileIndex];
                Image loadedImage;
                if (loadedImage.loadFromFile(selectedFile)) {
                    texture.loadFromImage(loadedImage);
                    window.display();
                }
                fileDialog.close();
                return true; 
                }
            }
        }

        fileDialog.clear(Color::White);
        
        int yOffset = 40;
        for (size_t i = 0; i < fileList.size(); ++i) {
            Text fileText(fileList[i], font, 16);
            fileText.setFillColor(Color::Black);
            fileText.setPosition(10, yOffset);
            if (i == selectedFileIndex) {
                
                fileText.setFillColor(Color::Red);
            }
            fileDialog.draw(fileText);
            yOffset += 20;
        }

        fileDialog.display();
    }
    return false;
}

std::string generateNextFileName(const std::string& folderPath, const std::string& baseName, const std::string& extension) {
    int fileNumber = 0;
    std::string filePath;

    do { 
        std::ostringstream oss;
        oss << folderPath << "/" << baseName << "_" << std::setw(3) << std::setfill('0') << fileNumber << extension;
        filePath = oss.str();
        fileNumber++;
    } while (fs::exists(filePath));

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
// Save å sånt klart

// ### - Action - ### 
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
        Vector2i mousePos = Mouse::getPosition(window);
        lineStartPreview = Vector2f(mousePos.x, mousePos.y);
        lineEndPreview = lineStartPreview;
        if (isDrawing) {
            Vector2i mousePos = Mouse::getPosition(window);
            lineStart.x = std::round((mousePos.x - 20 - 20) / scale) * scale + 20 + 20;
            lineStart.y = std::round((mousePos.y - 20) / scale) * scale + 20;
            lineStartPreview = Vector2f(mousePos.x, mousePos.y);
            lineEndPreview = lineStartPreview;
            lineEnd = lineStart;
        }
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
                            Texture texture;
                            texture.loadFromImage(image);
                            Sprite sprite(texture);
        }
    }
    if (event.key.code == Keyboard::Q && (event.key.control || event.key.system)) {
// Skapa en ny image att radera det gamla med
    }
}

void handleKeyReleasedEvent(const Event& event) {
    if (event.key.code == Keyboard::LShift || event.key.code == Keyboard::RShift) {
        isShiftKeyPressed = false;
        preview.setPosition(Vector2f(-scale, -scale));
        preview.setFillColor(Color::Transparent);
        
    }
}

void handleMouseButtonPressedEvent(const Event& event) {
    if (event.mouseButton.button == Mouse::Left || event.mouseButton.button == Mouse::Right) {
        isMousedown = true;

        if (event.mouseButton.button == Mouse::Left) {
            Vector2i mousePos = Mouse::getPosition(window);
            lineStartPreview = Vector2f(mousePos.x, mousePos.y);
            lineEndPreview = lineStartPreview;
            isDrawing = true;
            if (isShiftKeyPressed) {
                Vector2i mousePos = Mouse::getPosition(window);
                lineStart.x = (mousePos.x);
                lineStart.y = (mousePos.y);
                lineStartPreview = Vector2f(lineStart.x, lineStart.y);
                lineEndPreview = lineStartPreview;
                lineEnd = lineStart;
            } else if (menuFile.getGlobalBounds().contains(static_cast<Vector2f>(Mouse::getPosition(window)))) {
                isMenuOpen = !isMenuOpen;
            } else if (checkbox.getGlobalBounds().contains(static_cast<Vector2f>(Mouse::getPosition(window)))) {
                outlineColorOnOff = !outlineColorOnOff;
                create_preview();
            } else if (isDrawing && !isShiftKeyPressed) {
                handleDrawing(event);
            }
        }
        if (!menuFile.getGlobalBounds().contains(static_cast<Vector2f>(Mouse::getPosition(window)))) {// || !menuSaveText.getGlobalBounds().contains(static_cast<Vector2f>(Mouse::getPosition(window))) || !menuLoadText.getGlobalBounds().contains(static_cast<Vector2f>(Mouse::getPosition(window)))
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
        lineEndPreview = Vector2f(event.mouseMove.x, event.mouseMove.y);
    }
}

void handleMouseButtonReleasedEvent(const Event& event) {
    isDrawing = false;
    isErasing = false;
    isMousedown = false;

    if (isShiftKeyPressed) {
        Vector2i mousePos = Mouse::getPosition(window);
        lineEnd.x = mousePos.x;
        lineEnd.y = mousePos.y;
        
        drawShiftLines(event);
        preview.setPosition(Vector2f(-scale, -scale));
        preview.setFillColor(Color::Transparent);
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
        } else if (menuLoadText.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
            if (openAndLoadImage(image)) {
                                sf::Texture texture;
                                texture.loadFromImage(image);
                                sf::Sprite sprite(texture);
            }
            isMenuOpen = false;
        }
    }
}

// Drawing behöver fortfarande justeras in på rätt ställe!
void handleDrawing(const Event& event) {
    Vector2i mousePos = Mouse::getPosition(window);
    int x = (((mousePos.x)/scale) - static_cast<float>(scale));
    int y = (((mousePos.y)/scale) - static_cast<float>(scale));

    int x_nearest = static_cast<int>((x / scale) * scale);
    int y_nearest = static_cast<int>((y / scale) * scale);

    if (selectedBrushSize > brushSizeValueStart) {
        x_nearest -= (selectedBrushSize / 2);
        y_nearest -= (selectedBrushSize / 2);
    }

    for (int i = 0; i < selectedBrushSize; i++) {
        for (int j = 0; j < selectedBrushSize; j++) {
            if (sprite.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
                image.setPixel(x_nearest + i, y_nearest + j, penColor);
            } else {isDrawing = false;}
        }
    }
    saveImageToHistory();
    texture.loadFromImage(image);
}    

void drawShiftLines(const Event& event) {
    if (isShiftKeyPressed) {
        isDrawing = false;
        Vector2i mousePos = Mouse::getPosition(window);

        int lineStartPointX = static_cast<int>(lineStart.x / scale);
        int lineStartPointY = static_cast<int>(lineStart.y / scale);
        int lineEndPointX = static_cast<int>(lineEnd.x / scale);
        int lineEndPointY = static_cast<int>(lineEnd.y / scale);

        if (lineStartPointX != lineEndPointX || lineStartPointY != lineEndPointY) {
            float deltaX = static_cast<float>(lineEndPointX - lineStartPointX);
            float deltaY = static_cast<float>(lineEndPointY - lineStartPointY);
            float steps = std::max(std::abs(deltaX), std::abs(deltaY)) * scale;

            for (float t = 0.0f; t <= 1.0f; t += scale / steps) {
                int x = static_cast<int>(lineStartPointX + t * deltaX);
                int y = static_cast<int>(lineStartPointY + t * deltaY);

                int x_nearest = (static_cast<int>(x));
                int y_nearest = (static_cast<int>(y));
                float lineBrushSize = static_cast<float>((selectedBrushSize-1)/2);

                for (int i = x_nearest - lineBrushSize; i <= x_nearest + lineBrushSize; i += 1) {
                    for (int j = y_nearest - lineBrushSize; j <= y_nearest + lineBrushSize; j += 1) {
                        if (sprite.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
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
        lineStart = Vector2f(-scale, -scale);
        lineEnd = Vector2f(-scale, -scale);
    }
}

// ### - Renderar allt till fönstret - ###
void render() {
    window.clear(Color(bgColor));

    window.draw(menuBar);
    window.draw(menuFile);

    create_GridLines(window);

    for (const Text& text : leftRulerTexts) {
        window.draw(text);
    }
    for (const Text& text : topRulerTexts) {
        window.draw(text);
    }
    for (int i = 0; i < numSquaresTop; ++i) {
        window.draw(squareShapesTop[i]);
    } 
    for (int i = 0; i < numSquares; ++i) {
        window.draw(squareShapes[i]);
    }
    window.draw(topLine);
    window.draw(leftLine);
    window.draw(topRuler);
    window.draw(leftRuler);
    window.draw(leftRulerText);
    window.draw(topRulerText);

    window.draw(previewScreen);
    window.draw(infoSpace);
    window.draw(checkbox);
    window.draw(checkboxInfoText);

    if (outlineColorOnOff) {
        window.draw(checkmark);
    }

    window.draw(brushSize);
    window.draw(brushSizeSmall);
    window.draw(brushSizeMed);
    window.draw(brushSizeMax);

    // Jag måste skapa min visuella brush här nere för loopen...
        Vector2i mousePos = Mouse::getPosition(window);
        FloatRect spriteBounds = sprite.getGlobalBounds();
        float brushSizeValue = selectedBrushSize * scale;
        CircleShape centralPoint(2.0f);
        VertexArray lineX(Lines, 4);
        VertexArray lineY(Lines, 4);

        int scaleSq = scale; // Passar in den i rutnätet
        int x_nearest = static_cast<int>(static_cast<float>((mousePos.x / scaleSq) * scaleSq) + selectedBrushSize);
        int y_nearest = static_cast<int>(static_cast<float>((mousePos.y / scaleSq) * scaleSq) + selectedBrushSize);

        if (sprite.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {

            brush.setSize(Vector2f(brushSizeValue, brushSizeValue));
            brush.setPosition(static_cast<int>(x_nearest) - brushSizeValue, static_cast<int>(y_nearest) - brushSizeValue);
            brush.setFillColor(mutedWhite);

            centralPoint.setFillColor(Color(elementColorAlfa));
            lineX[0].color = Color(elementColorAlfa);
            lineX[1].color = Color(elementColorAlfa);
            lineX[2].color = Color(elementColorAlfa);
            lineX[3].color = Color(elementColorAlfa);

            lineY[0].color = Color(elementColorAlfa);
            lineY[1].color = Color(elementColorAlfa);
            lineY[2].color = Color(elementColorAlfa);
            lineY[3].color = Color(elementColorAlfa);

            centralPoint.setOrigin(centralPoint.getRadius(), centralPoint.getRadius());
            centralPoint.setPosition(static_cast<float>(brush.getPosition().x) +(brushSizeValue/2)-selectedBrushSize, static_cast<float>(brush.getPosition().y) +(brushSizeValue/2)-selectedBrushSize);
            
            int lineLengthCross = 30;
            int lineCenterCross = (brushSizeValueStart * scale);
            int lineEndCross = (lineLengthCross + brushSizeValue);
            lineX[0].position = sf::Vector2f((brush.getPosition().x) - lineCenterCross - selectedBrushSize,                  (brush.getPosition().y) + (brushSizeValue / 2) - selectedBrushSize);
            lineX[1].position = sf::Vector2f((brush.getPosition().x) - lineEndCross,     (brush.getPosition().y) + (brushSizeValue / 2) - selectedBrushSize);
            lineX[2].position = sf::Vector2f((brush.getPosition().x) + lineCenterCross + brushSizeValue - selectedBrushSize,              (brush.getPosition().y) + (brushSizeValue / 2) - selectedBrushSize);
            lineX[3].position = sf::Vector2f((brush.getPosition().x) + lineEndCross + brushSizeValue,     (brush.getPosition().y) + (brushSizeValue / 2) - selectedBrushSize);

            lineY[0].position = sf::Vector2f((brush.getPosition().x) + (brushSizeValue / 2) - selectedBrushSize, (brush.getPosition().y) - lineCenterCross - selectedBrushSize);
            lineY[1].position = sf::Vector2f((brush.getPosition().x) + (brushSizeValue / 2) - selectedBrushSize, (brush.getPosition().y) - lineEndCross);
            lineY[2].position = sf::Vector2f((brush.getPosition().x) + (brushSizeValue / 2) - selectedBrushSize, (brush.getPosition().y) + lineCenterCross + brushSizeValue - selectedBrushSize);
            lineY[3].position = sf::Vector2f((brush.getPosition().x) + (brushSizeValue / 2) - selectedBrushSize, (brush.getPosition().y) + lineEndCross + brushSizeValue);
        }
    // Borste slut
    window.draw(lineY);             
    window.draw(lineX);
    window.draw(brush);
    window.draw(centralPoint);
    updateBrush();




    if (isErasing) {
        window.draw(infoSymbolErase);
    }

    if (isMenuOpen) {
        window.draw(dropDown);
        window.draw(menuSaveText);
        window.draw(menuLoadText);
    }

// previewlinjer för shiftline
    RectangleShape preview(Vector2f(scale, scale));
    if (isShiftKeyPressed) {

        int lineStartPointX = static_cast<int>(lineStartPreview.x / scale);
        int lineStartPointY = static_cast<int>(lineStartPreview.y / scale);
        int lineEndPointX = static_cast<int>(lineEndPreview.x / scale);
        int lineEndPointY = static_cast<int>(lineEndPreview.y / scale);

        if (lineStartPointX != lineEndPointX || lineStartPointY != lineEndPointY) {
            float deltaX = static_cast<float>(lineEndPointX - lineStartPointX);
            float deltaY = static_cast<float>(lineEndPointY - lineStartPointY);
            float steps = std::max(std::abs(deltaX), std::abs(deltaY)) * scale;

            for (float t = 0.0f; t <= 1.0f; t += scale / steps) {
                int x = static_cast<int>(lineStartPointX + t * deltaX)*scale;
                int y = static_cast<int>(lineStartPointY + t * deltaY)*scale;

                int x_nearest = (((static_cast<int>(x))/scale)*scale)-(selectedBrushSize);
                int y_nearest = (((static_cast<int>(y))/scale)*scale)-(selectedBrushSize);


                float lineBrushSize = static_cast<float>((selectedBrushSize)/2);

                for (int i = x_nearest - lineBrushSize; i <= x_nearest + lineBrushSize; i += 1) {
                    for (int j = y_nearest - lineBrushSize; j <= y_nearest + lineBrushSize; j += 1) {
                        if (sprite.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {

                            preview.setPosition(Vector2f(i, j));
                            preview.setFillColor(Color(penColor));
                            window.draw(preview);
                    }   }
                }
            }
        }
    }
    preview.setPosition(Vector2f(-scale, -scale)); 
    preview.setFillColor(Color::Transparent);
// preview slut

    sprite.setPosition(rulerSize, extraTopHeight); 
    window.draw(sprite);
    window.draw(previewSprite);
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