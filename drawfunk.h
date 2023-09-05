#ifndef DRAWFUNK_H
#define DRAWFUNK_H

#pragma once
#include <SFML/Graphics.hpp>
#include "drawfunk.cpp"

class DrawFunk {
public:
    
    DrawFunk(sf::RenderWindow& window, sf::RenderTexture& renderTexture, sf::Image& canvas, sf::RectangleShape& brush, sf::RectangleShape& brushSizeSmall, sf::RectangleShape& brushSizeMed, sf::RectangleShape& brushSizeMax, int scale, bool& isDrawing, bool& isErasing, bool& isShiftKeyPressed, sf::Vector2f& lineStart, float& selectedBrushSize);

    
    void changeBrushSize(float newSize);
    void ifShift();
    void DrawErase();
    void HandleMouseButton();
    void HandleBrushSize();
    void HandleDrawing();
    void HandleMouseButtonReleased(const sf::Event& event);
    void HandleShiftLines();
    void ShiftlineVisual(sf::RenderWindow& window);
 
private:
    
    sf::RenderWindow& window;
    sf::RenderTexture& renderTexture;
    sf::Image& canvas;
    sf::RectangleShape& brush;
    sf::RectangleShape& brushSizeSmall;
    sf::RectangleShape& brushSizeMed;
    sf::RectangleShape& brushSizeMax;
    int scale;
    bool& isDrawing;
    bool& isErasing;
    bool& isMousedown;
    bool& isShiftKeyPressed;
    sf::Vector2f& lineStart;
    float& selectedBrushSize;
    int x;
    int y;
};

#endif 
