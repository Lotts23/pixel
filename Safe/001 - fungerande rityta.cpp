#include <SFML/Graphics.hpp>

class DrawingApp {
public:
    DrawingApp() : window(sf::VideoMode(512 + 25, 256 + 25), "Ritprogram") {


        
        createLines();

        
        createPatterns();

        
        if (!font.loadFromFile("/System/Library/Fonts/Supplemental/Courier New.ttf")) {
            
            throw std::runtime_error("Unable to load font");
        }
        
        
        createText();

        
        image.create(128, 64, sf::Color::White); 


        
        sprite.setScale(4.0f, 4.0f);

        
        texture.loadFromImage(image);
        sprite.setTexture(texture, true);
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            render();
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

void createLines() {
    topLine.setSize(sf::Vector2f(512, 1));
    topLine.setFillColor(sf::Color::Black);
    topLine.setPosition(24, 24); 

    leftLine.setSize(sf::Vector2f(1, 256));
    leftLine.setFillColor(sf::Color::Black);
    leftLine.setPosition(24, 24); 

    leftRuler.setSize(sf::Vector2f(23, 256));
    leftRuler.setFillColor(sf::Color::Transparent);
    leftRuler.setPosition(0, 24); 

    topRuler.setSize(sf::Vector2f(512, squareSize)); 
    topRuler.setFillColor(sf::Color::Transparent);
    topRuler.setPosition(24, 1); 
}

    void createPatterns() {
        leftRulerPattern.setSize(sf::Vector2f(23, 256));
        leftRulerPattern.setFillColor(sf::Color::Transparent);

        topRulerPattern.setSize(sf::Vector2f(512, 20));
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

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    int x = (mousePos.x - 25) / 4; 
                    int y = (mousePos.y - 25) / 4;
                    if (x >= 0 && x < 128 && y >= 0 && y < 64) {
                        image.setPixel(x, y, sf::Color::Black);
                        texture.loadFromImage(image);
                    }
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    
                }
            }
        }
    }

    void render() {
        window.clear(sf::Color::White);
        
        for (int i = 0; i < numSquares; ++i) {
            sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));
            if (i % 2 == 0) {
                square.setFillColor(sf::Color::Black); 
            } else {
                square.setFillColor(sf::Color::White); 
            }

            square.setPosition(20, 25 + (i * squareSize));
            window.draw(square);
        }

        window.draw(leftRuler);
        window.draw(topRuler);
        window.draw(topLine);
        window.draw(leftLine);
        sprite.setPosition(25, 25); 
        window.draw(sprite);
        window.draw(leftRulerText);
        window.draw(topRulerText);

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
