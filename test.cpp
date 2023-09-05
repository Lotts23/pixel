#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>

using namespace sf;

class DrawingApp {
public:
    DrawingApp(int initialSizeX = 400, int initialSizeY = 300) : window(VideoMode(initialSizeX, initialSizeY), "Ritprogram") {
        if (!renderTexture.create(initialSizeX, initialSizeY)) {
            return;
        }

        brush.setSize(Vector2f(10, 10));
        brush.setOrigin(0, 0);
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            render();
        }
    }

private:
    RenderWindow window;
    RenderTexture renderTexture;
    Clock clock;
    RectangleShape brush;
    Sprite canvas;

    void handleEvents() {
        Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case Event::Closed:
                    window.close();
                    break;

                case Event::MouseButtonPressed:
                    if (event.mouseButton.button == Mouse::Left) {
                        handleMouseClick();
                    }
                    break;

                default:
                    break;
            }
        }
    }

    void handleMouseClick() {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        brush.setPosition(static_cast<sf::Vector2f>(mousePos));
        renderTexture.draw(brush);
    }

    void render() {
        Time elapsedTime = clock.getElapsedTime();
        if (elapsedTime.asSeconds() >= 0.1) {
            window.clear();
            canvas.setTexture(renderTexture.getTexture());
            window.draw(canvas);
            window.display();
            clock.restart();
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
};

int main() {
    DrawingApp app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
