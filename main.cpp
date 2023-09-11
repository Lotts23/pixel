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
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }
            Time elapsedTime = frameClock.getElapsedTime();
            if (elapsedTime.asSeconds() >= frameTime) {
                render();
                frameClock.restart();
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }

private:
    const float scale = 8.0f;
    const int baseSizeX = 128;
    const int baseSizeY = 64;

    Color bgColor = Color(85, 107, 47);
    Color elementColor = Color(60, 60, 60);

    int sizeX = (baseSizeX * static_cast<int>(scale));
    int sizeY = (baseSizeY * static_cast<int>(scale));    
    RectangleShape menuBar;
    Text menuFile;
    Font font;
    Vector2f menuSize = Vector2f(sizeX + (scale*6), scale*3);
    float rulerSize = menuSize.y * 2;
    float textSpacing = scale;
    float lineWidth = 1.0;
    float extraHeight = menuSize.y + rulerSize + lineWidth;

    int initialSizeX = sizeX + rulerSize;
    int initialSizeY = sizeY + extraHeight;
    RenderWindow window;

    Image image;
    Texture texture;
    Sprite sprite;

    RectangleShape topRuler;
    RectangleShape topLine;
    RectangleShape leftRuler;
    RectangleShape leftLine;
    Text leftRulerText;
    Text topRulerText;
    std::vector<Text> leftRulerTexts;
    std::vector<Text> topRulerTexts;
    const float squareSize = scale;
    RectangleShape squareShapes[64];
    RectangleShape squareShapesTop[128];
    const int numSquares = std::extent<decltype(squareShapes)>::value;
    const int numSquaresTop = std::extent<decltype(squareShapesTop)>::value;

    Clock frameClock;
    const float frameTime = 0.1f;
/* 
    std::vector<RectangleShape> squareShapes; Jag hade velat ha dessa flexibla!
    std::vector<RectangleShape> squareShapesTop;  
    
    const int numSquares = sizeof(squareShapes) / sizeof(squareShapes[0]);
    const int numSquaresTop = sizeof(squareShapesTop) / sizeof(squareShapesTop[0]);
    */

void initialize() {
    image.create((sizeX), (sizeY), Color::Blue);


    if (!font.loadFromFile("/System/Library/Fonts/Supplemental/Courier New.ttf")) {
        throw std::runtime_error("Unable to load font");
    }

    create_menuBar();
    createRulers();
    createText();
    createTopSquareShapes();
    createLeftSquareShapes();

    frameClock.restart();
}

/*
     Det här hade varit bättre sätt att ange storleken i initialize
    RectangleShape squareShapes[baseSizeY];
    RectangleShape squareShapesTop[baseSizeX];
  */
void create_menuBar() {
    menuBar.setSize(menuSize);
    menuBar.setFillColor(Color::White);
    menuBar.setPosition(0, 0);

    create_menuText();
}

void create_menuText() {
    menuFile.setFont(font);
    menuFile.setCharacterSize(12);
    menuFile.setString("File");
    menuFile.setPosition(menuSize.y, menuSize.y - textSpacing - menuFile.getCharacterSize());
    menuFile.setFillColor(Color::Black);
}

void createRulers() {
    topRuler.setSize(Vector2f(sizeX, rulerSize));
    topRuler.setFillColor(Color::Transparent);
    topRuler.setPosition(rulerSize, menuSize.y);

    topLine.setSize(Vector2f(sizeX, lineWidth));
    topLine.setFillColor(elementColor);
    topLine.setPosition(rulerSize, extraHeight - lineWidth);

    leftRuler.setSize(Vector2f(rulerSize, sizeY));
    leftRuler.setFillColor(Color::Transparent);
    leftRuler.setPosition(0, extraHeight);

    leftLine.setSize(Vector2f(lineWidth, sizeY));
    leftLine.setFillColor(elementColor);
    leftLine.setPosition(rulerSize, extraHeight);
}

void createTopSquareShapes() { // En extra ruta till 0:an
    int zeroBase = (baseSizeX + 2);
    for (int i = 0; i < zeroBase; ++i) {
        RectangleShape square;
        square.setSize(Vector2f(squareSize, squareSize));
        if ((i - baseSizeX) % 2 == 0) {
            square.setFillColor(Color::Transparent);
        } else {
            square.setFillColor(elementColor);
        }
        square.setPosition(rulerSize + (i * squareSize), extraHeight - squareSize - lineWidth);
        squareShapesTop[i] = square;
    } 
}

void createLeftSquareShapes() {
    for (int i = 0; i < baseSizeY; ++i) {
        RectangleShape square;
        square.setSize(Vector2f(squareSize, squareSize));
        if (i % 2 == 0) {
            square.setFillColor(Color::Transparent);
        } else {
            square.setFillColor(elementColor);
        }
        square.setPosition(rulerSize - squareSize, extraHeight + (i * squareSize));
        squareShapes[i] = square;
    }
}

void createText() {
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
        rulerText.setPosition(rulerSize / 2 + 6, extraHeight - 12 + (i * squareSize));
        } else {
        rulerText.setPosition(rulerSize / 2, extraHeight - 12 + (i * squareSize));
        }
        leftRulerTexts.push_back(rulerText);
    }
}

// #########
void render() {
    window.clear(Color(bgColor));
    window.draw(menuBar);
    window.draw(menuFile);

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

    sprite.setPosition(rulerSize, extraHeight); 
    window.draw(sprite);
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