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

    RectangleShape brush;

    int sizeX = initialSizeX - scale*2 + (scale/2);
    int sizeY = initialSizeY - scale*2 + (scale/2);
    int startPixelX;
    int startPixelY;
    VertexArray currentSegment;
    Vector2f previewLineStart;
    Vector2f previewLineEnd;


    bool isShiftKeyPressed;

    bool isDrawing;
    bool isMousedown;

    Clock clock;
    Vector2f lineStart;
    Vector2f lineEnd;
    Color bgColor;

    Color penColor = Color::Black;

    float brushSizeValue = 1;


    void initialize() {

        sizeX = ((window.getSize().x)- ((scale*2 + (scale/2))*scale))/scale + 1;
        sizeY = ((window.getSize().y)-((scale*2 + (scale/2))*scale))/scale + 1;

        Color bgColor(85, 107, 47);
        image.create((sizeX), (sizeY), Color::Transparent);

        texture.loadFromImage(image);
        sprite.setTexture(texture, true);
        sprite.setScale(scale, scale);

        updateBrush();

        isShiftKeyPressed = false;
        lineStart = Vector2f(-scale, -scale);
        lineEnd = Vector2f(-scale, -scale);
        isDrawing = false;

        clock.restart();
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
                Vector2i mousePos = Mouse::getPosition(window);
                lineStart.x = std::round((mousePos.x - 20 - 20) / scale) * scale + 20 + 20;
                lineStart.y = std::round((mousePos.y - 20) / scale) * scale + 20;
                lineEnd = lineStart;
                previewLineStart.x = std::round((mousePos.x - 20 - 20) / scale) * scale + 20 + 20;
                previewLineStart.y = std::round((mousePos.y - 20) / scale) * scale + 20;

            } else {
                handleDrawing(event);
            }
        }
    }
}

void handleMouseMovedEvent(const Event& event) {
    if (isDrawing && !isShiftKeyPressed) {
        handleDrawing(event);
    }
    if (isMousedown && isShiftKeyPressed) {
        Vector2i mousePos = Mouse::getPosition(window);
        previewLineEnd.x = std::round((mousePos.x - 20 - 20) / scale) * scale + 20 + 20;
        previewLineEnd.y = std::round((mousePos.y - 20) / scale) * scale + 20;
    }
}


void handleMouseButtonReleasedEvent(const Event& event) {
    isDrawing = false;
    isMousedown = false;
    if (isShiftKeyPressed) {
        Vector2i mousePos = Mouse::getPosition(window);
        lineEnd.x = std::round((mousePos.x - 20 - 20) / scale) * scale + 20 + 20;
        lineEnd.y = std::round((mousePos.y - 20) / scale) * scale + 20;
        
        drawShiftLines(event);
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
                    image.setPixel(i, j, penColor);
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

                                    image.setPixel(i, j, penColor);
                
                                    while (startPixelX == !endPixelX && startPixelY == !endPixelY) {
                                        VertexArray currentSegment(sf::Quads, 4);

                                        currentSegment[0].position = lineStart;
                                        currentSegment[1].position = lineEnd;
                                        currentSegment[2].position = lineEnd;
                                        currentSegment[3].position = lineStart;

                                        for (int i = 0; i < 4; ++i) {
                                            currentSegment[i].color = sf::Color::White;
                                        }
                                        window.draw(currentSegment);
                                    }


                            }
                        }
                    }
                }
                texture.loadFromImage(image);

            }
        }
        
        currentSegment.clear();
        
        lineStart = Vector2f(-scale, -scale);
        lineEnd = Vector2f(-scale, -scale);
    }
}



void updateBrush() {
    brush.setSize(Vector2f(scale, scale));
    brush.setOrigin(brush.getSize());
}

void render() {
    window.clear(Color(85, 107, 47));
    sprite.setPosition((scale * 6), (scale * 11));
    window.draw(sprite);

    if (isShiftKeyPressed) {
        VertexArray previewLine(sf::Lines, 2);
        previewLine[0].position = previewLineStart;
        previewLine[1].position = previewLineEnd;
        previewLine[0].color = penColor;
        previewLine[1].color = penColor;
        window.draw(previewLine);
    }

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
