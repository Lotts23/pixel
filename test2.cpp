#include <SFML/Graphics.hpp>
#include <thread>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <filesystem>
using namespace sf;

class DrawingApp {
public:
    DrawingApp(int initialSizeX = (128 + 20) * 8, int initialSizeY = (64 + 20) * 8) : window(VideoMode(initialSizeX, initialSizeY), "Ritprogram") {
        

        Color bgColor(85, 107, 47);
        image.create((scale * 32) - 1, (scale * 16) - 1, Color::White); 

        selectedBrushSize = scale;
        sprite.setScale(selectedBrushSize, selectedBrushSize);

        image.copy(memoryImage, 0, 0);

        texture.loadFromImage(image);
        sprite.setTexture(texture, true);

        clock.restart();

        create_infoSpace();

        create_infoSymbols();
        create_infoBrushes();

        isDrawing = false;
        isErasing = false;
        sizeX = initialSizeX;
        sizeY = initialSizeY;
        int punkter = 8 * 8;
        sf::VertexArray brush(sf::Points, punkter);
        
        
        std::cout << "Constructor: initialSizeX = " << initialSizeX << ", initialSizeY = " << initialSizeY << std::endl;
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
        }
    }

private:
    RenderWindow window;

    RectangleShape squareShapes[64];
    RectangleShape squareShapesTop[128];
    float scale = 8.0f;
    int sizeX;
    int sizeY;
    VertexArray brush;
    float selectedBrushSize;
    Image image;
    Image memoryImage;
    Texture texture;
    Sprite sprite;
    RectangleShape menuBar;
    RectangleShape dropDown;
    Text menuFile;
    Text menuText;
    Text menuSaveText;
    RectangleShape topLine;
    RectangleShape leftLine;
    RectangleShape leftRuler;
    RectangleShape topRuler;
    RectangleShape leftRulerPattern;
    RectangleShape topRulerPattern;
    Font font;
    Text leftRulerText;
    Text topRulerText;
    const float squareSize = (scale-1);
    Texture rulerPatternTexture;
    bool isMenuOpen;
    bool isShiftKeyPressed;
    RectangleShape menuBackground;
    bool isSaveButtonClicked;
    Clock saveButtonClock;
    bool isDrawing;
    bool isErasing;
    bool isMousedown;
    const int numSquares = std::extent<decltype(squareShapes)>::value;
    const int numSquaresTop = std::extent<decltype(squareShapesTop)>::value;
    std::vector<Text> leftRulerTexts;
    std::vector<Text> topRulerTexts;
    Clock clock;


    Color bgColor;

    RectangleShape infoSpace;
    Text infoSymbolErase;
    RectangleShape brushSize;
    RectangleShape brushSizeSmall;
    RectangleShape brushSizeMed;
    RectangleShape brushSizeMax;



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


void changeBrushSize(float newSize) {

    selectedBrushSize = newSize;

    

    std::cout << selectedBrushSize << ", " << image.getSize().x << ", " << image.getSize().y << std::endl;
}



    void DrawErase() {      
        isDrawing = !isDrawing;
        isErasing = !isErasing;
        }  

    void handleEvents() {
        Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case Event::Closed:
                    window.close();
                    break;
                case Event::KeyPressed:
                    if (event.key.code == Keyboard::X) {
                        DrawErase();
                    }
                    break;
                case Event::MouseButtonPressed:
                    if (event.mouseButton.button == Mouse::Left || event.mouseButton.button == Mouse::Right) {
                        Vector2i mousePos = Mouse::getPosition(window);


                    
                    int x = (mousePos.x - (scale*6)+1) / scale;
                    int y = (mousePos.y - (scale*11)+1) / scale;
                    std::cout << "Mouse pressed: x = " << x << ", y = " << y << std::endl;


                        if (event.mouseButton.button == Mouse::Left) {
                            
                            if (brushSizeSmall.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                changeBrushSize(scale);
                            } else if (brushSizeMed.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                changeBrushSize(scale * 2);
                            } else if (brushSizeMax.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                changeBrushSize(scale * 3);
                            }   
                        }


                        
                        if (event.mouseButton.button == Mouse::Left) {
                            isMousedown = true; 
                            isDrawing = true;
                        }

                        
                        if (event.mouseButton.button == Mouse::Right) {
                            isErasing = true;
                        }


                    
                    brush = createBrush(selectedBrushSize);
                    }
                    break;
                    
                case Event::MouseMoved:


                    if ((event.mouseButton.button == Mouse::Left || isMousedown) && (isDrawing || isErasing) && !isShiftKeyPressed) {
                            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                            int x = (mousePos.x - (scale * 6) + 1) / selectedBrushSize;
                            int y = (mousePos.y - (scale * 11) + 1) / selectedBrushSize;
                            std::cout << "Mouse moved: x = " << x << ", y = " << y << std::endl;

                            if (x >= 0 && x < 128 && y >= 0 && y < 64) {

                                if (isErasing) {
                                    image.setPixel(x, y, sf::Color::Transparent);
                                } else {
                                    
                                    for (int i = 0; i < brush.getVertexCount(); i++) {
                                        int brushX = x * selectedBrushSize + static_cast<int>(brush[i].position.x);
                                        int brushY = y * selectedBrushSize + static_cast<int>(brush[i].position.y);
                                        if (brushX >= 0 && brushX < (128) && brushY >= 0 && brushY < (64)) {
                                            image.setPixel(brushX, brushY, sf::Color::Black); 
                                        }
                                    }
                                }

                                
                                texture.loadFromImage(image);
                            }
                        }
                break;
                case Event::MouseButtonReleased:
                isDrawing = false;
                isErasing = false;
                isMousedown = false;
                    break;
                default:
                 break;
            }
        }
    }

VertexArray createBrush(float brushSize) {
    int punkter = static_cast<int>(brushSize) * static_cast<int>(brushSize);
    sf::VertexArray brush(sf::Points, punkter);

    
    for (int y = 0; y < brushSize; ++y) {
        for (int x = 0; x < brushSize; ++x) {
            int index = y * brushSize + x;
            brush[index].position = sf::Vector2f(x, y);
        }
    }

    return brush;
}

    void render() {
        
        window.clear(Color(85, 107, 47));

        sprite.setPosition((scale*6)+1, (scale*11)+1);

        window.draw(sprite);
        window.draw(brush);
        
        window.draw(infoSpace);
        window.draw(brushSize);
        window.draw(brushSizeSmall);
        window.draw(brushSizeMed);
        window.draw(brushSizeMax);
        
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
