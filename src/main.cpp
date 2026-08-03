#include <SFML/Graphics.hpp>
#include <iostream>
#include "core/Grid.h"
#include "core/Constants.h"
#include "rendering/MazeRenderer.h"
#include "rendering/Colors.h"
#include "ui/InfoPanel.h"

// ============================================
// CONVERT MOUSE PIXEL POSITION TO GRID CELL
// ============================================
void mouseToGrid(int mouseX, int mouseY, int& row, int& col) {
    col = mouseX / CELL_SIZE;
    row = mouseY / CELL_SIZE;
}

// ============================================
// PLACEMENT MODES
// ============================================
enum class PlaceMode {
    WALL,
    START,
    END,
    ERASE
};

std::string modeToString(PlaceMode mode) {
    switch (mode) {
        case PlaceMode::WALL:  return "WALL";
        case PlaceMode::START: return "START";
        case PlaceMode::END:   return "END";
        case PlaceMode::ERASE: return "ERASE";
    }
    return "WALL";
}

int main() {
    // Create window
    sf::RenderWindow window(
        sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
        "PathForge - Pathfinding Engine"
    );
    window.setFramerateLimit(60);

    // Core objects
    Grid grid;
    MazeRenderer renderer;
    InfoPanel panel;

    // Load font
    if (!panel.loadFont()) {
        std::cout << "WARNING: Could not load font. Panel text will not appear." << std::endl;
        std::cout << "Copy any .ttf font to assets/fonts/ folder." << std::endl;
    }

    PlaceMode currentMode = PlaceMode::WALL;
    panel.setMode(modeToString(currentMode));

    bool isDragging = false;

    std::cout << "===================================" << std::endl;
    std::cout << "  PathForge started!" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "Look at the sidebar for controls." << std::endl;

    auto applyMode = [&](int row, int col) {
        if (!grid.inBounds(row, col)) return;

        switch (currentMode) {
            case PlaceMode::WALL:
                grid.setWall(row, col);
                break;
            case PlaceMode::START:
                grid.setStart(row, col);
                break;
            case PlaceMode::END:
                grid.setEnd(row, col);
                break;
            case PlaceMode::ERASE:
                Cell& cell = grid.getCell(row, col);
                if (!cell.isStart() && !cell.isEnd()) {
                    cell.terrain = Terrain::EMPTY;
                }
                break;
        }
    };

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Mouse press
            if (event.type == sf::Event::MouseButtonPressed) {
                int row, col;
                mouseToGrid(event.mouseButton.x, event.mouseButton.y, row, col);

                if (event.mouseButton.x >= MAZE_WIDTH) continue; // ignore panel clicks

                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (grid.inBounds(row, col)) {
                        if (currentMode == PlaceMode::WALL) {
                            grid.toggleWall(row, col);
                        } else {
                            applyMode(row, col);
                        }
                        isDragging = true;
                    }
                }

                if (event.mouseButton.button == sf::Mouse::Right) {
                    if (grid.inBounds(row, col)) {
                        grid.setStart(row, col);
                    }
                }

                if (event.mouseButton.button == sf::Mouse::Middle) {
                    if (grid.inBounds(row, col)) {
                        grid.setEnd(row, col);
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isDragging = false;
                }
            }

            // Mouse move - handle hover and dragging
            if (event.type == sf::Event::MouseMoved) {
                int row, col;
                mouseToGrid(event.mouseMove.x, event.mouseMove.y, row, col);

                if (event.mouseMove.x < MAZE_WIDTH && grid.inBounds(row, col)) {
                    renderer.setHover(row, col);

                    if (isDragging) {
                        if (currentMode == PlaceMode::WALL) {
                            grid.setWall(row, col);
                        } else {
                            applyMode(row, col);
                        }
                    }
                } else {
                    renderer.clearHover();
                }
            }

            // Keyboard
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::W) {
                    currentMode = PlaceMode::WALL;
                    panel.setMode("WALL");
                }
                if (event.key.code == sf::Keyboard::S) {
                    currentMode = PlaceMode::START;
                    panel.setMode("START");
                }
                if (event.key.code == sf::Keyboard::E) {
                    currentMode = PlaceMode::END;
                    panel.setMode("END");
                }
                if (event.key.code == sf::Keyboard::X) {
                    currentMode = PlaceMode::ERASE;
                    panel.setMode("ERASE");
                }
                if (event.key.code == sf::Keyboard::C) {
                    grid.clearAll();
                }
                if (event.key.code == sf::Keyboard::G) {
                    grid.clearVisuals();
                }
            }
        }

        // Render
        window.clear(Colors::BG_DARK);
        renderer.render(window, grid);
        panel.render(window);
        window.display();
    }

    return 0;
}