#include "drawfunk.h" 
#pragma once

DrawFunk::DrawFunk(sf::RenderWindow& window, sf::RenderTexture& renderTexture, sf::Image& canvas, sf::RectangleShape& brush, sf::RectangleShape& brushSizeSmall, sf::RectangleShape& brushSizeMed, sf::RectangleShape& brushSizeMax, int scale, bool& isDrawing, bool& isErasing, bool& isShiftKeyPressed, sf::Vector2f& lineStart, float& selectedBrushSize)
    : window(window), renderTexture(renderTexture), canvas(canvas), brush(brush), brushSizeSmall(brushSizeSmall), brushSizeMed(brushSizeMed), brushSizeMax(brushSizeMax), scale(scale), isDrawing(isDrawing), isErasing(isErasing), isShiftKeyPressed(isShiftKeyPressed), lineStart(lineStart), selectedBrushSize(selectedBrushSize) {
}



void DrawFunk::changeBrushSize(float newSize) {
    selectedBrushSize = newSize;
    brush.setSize(sf::Vector2f(selectedBrushSize, selectedBrushSize));
}


void DrawFunk::ifShift() {
    isShiftKeyPressed = true;
    if (isDrawing) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        lineStart = sf::Vector2f(mousePos);
    }
}


void DrawFunk::DrawErase() {
    isDrawing = !isDrawing;
    isErasing = !isErasing;
}


void DrawFunk::HandleMouseButton() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    int x = (mousePos.x - (scale * 6) + 1) / scale;
    int y = (mousePos.y - (scale * 11) + 1) / scale;
    brush.setPosition(static_cast<sf::Vector2f>(mousePos));
    renderTexture.draw(brush);
}


void DrawFunk::HandleBrushSize() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    
    if (brushSizeSmall.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
        changeBrushSize(scale);
    } else if (brushSizeMed.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
        changeBrushSize(scale * 2);
    } else if (brushSizeMax.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
        changeBrushSize(scale * 3);
    }
}


void DrawFunk::HandleDrawing() {
    if (x >= 0 && x < 128 && y >= 0 && y < 64) {
        if (isErasing) {
            canvas.setPixel(x, y, sf::Color::Transparent);
        } else {
            canvas.setPixel(x, y, sf::Color::Black);
        }
        renderTexture.draw(brush);
    }
}


void DrawFunk::HandleMouseButtonReleased(const sf::Event& event) {
    if (event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right) {
    Vector2i mousePos = sf::Mouse::getPosition(window);
    isDrawing = false;
    isErasing = false;
    isMousedown = false;
    }
}

void DrawFunk::HandleShiftLines() {
    isDrawing = false;

    int startX = static_cast<int>((lineStart.x - (scale * 6) + 1) / scale) * scale + (scale * 6) + 1;
    int startY = static_cast<int>((lineStart.y - (scale * 6) + 1) / scale) * scale + (scale * 6) + 1;
    int endX = static_cast<int>((lineEnd.x - (scale * 6) + 1) / scale) * scale + (scale * 6) + 1;
    int endY = static_cast<int>((lineEnd.y - (scale * 6) + 1) / scale) * scale + (scale * 6) + 1;

    while (startX != endX || startY != endY) {
        VertexArray currentSegment(sf::Quads, 4);

        currentSegment[0].position = Vector2f(startX, startY);
        currentSegment[1].position = Vector2f(startX + (scale - 1), startY);
        currentSegment[2].position = Vector2f(startX + (scale - 1), startY + (scale - 1));
        currentSegment[3].position = Vector2f(startX, startY + (scale - 1));

        for (int i = 0; i < 4; ++i) {
            currentSegment[i].color = sf::Color::Black;
        }

        lines.push_back(currentSegment);

        if (startX < endX) {
            startX += scale;
        }
        else if (startX > endX) {
            startX -= scale;
        }
        if (startY < endY) {
            startY += scale;
        }
        else if (startY > endY) {
            startY -= scale;
        }
    }

    lineStart = Vector2f(-1, -1);
    lineEnd = Vector2f(-1, -1);
}


void DrawFunk::ShiftlineVisual(RenderWindow& window) {
    if (isDrawing && isShiftKeyPressed) {

        if (isDrawing && lineStart == Vector2f(-1, -1)) {
            Vector2i mousePos = Mouse::getPosition(window);
            lineStart = Vector2f(mousePos);
        }
        
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

            window.draw(currentSegment);

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
    }
}
