#include <SFML/Graphics.hpp>
#include <iostream>
#include "core/Grid.h"
#include "core/Constants.h"
#include "rendering/MazeRenderer.h"
#include "rendering/Colors.h"
#include "ui/InfoPanel.h"
#include "ui/StartupScreen.h"
#include "mazes/MazeLoader.h"

void mouseToGrid(int mouseX, int mouseY, int& row, int& col) {
    col = mouseX / CELL_SIZE;
    row = mouseY / CELL_SIZE;
}

enum class PlaceMode { WALL, START, END, ERASE };
enum class AppState { STARTUP, RUNNING };

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
    sf::RenderWindow window(
        sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
        "PathForge - Pathfinding Engine"
    );
    window.setFramerateLimit(60);

    Grid grid;
    MazeRenderer renderer;
    InfoPanel panel;
    StartupScreen startupScreen;

    if (!panel.loadFont()) std::cout << "WARNING: Panel font missing" << std::endl;
    if (!startupScreen.loadFont()) std::cout << "WARNING: Startup font missing" << std::endl;

    // App state
    AppState appState = AppState::STARTUP;
    GameMode gameMode = GameMode::CUSTOM;
    PlaceMode currentMode = PlaceMode::WALL;
    Difficulty currentDifficulty = Difficulty::BEGINNER;
    int currentLevel = 1;
    bool isDragging = false;

    std::cout << "===================================" << std::endl;
    std::cout << "  PathForge Started" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "Choose mode on the startup screen:" << std::endl;
    std::cout << "  Press 1 -> CUSTOM MODE" << std::endl;
    std::cout << "  Press 2 -> PRESET MODE" << std::endl;
    std::cout << "  Press ESC to quit anytime" << std::endl;
    std::cout << "===================================" << std::endl;

    auto applyMode = [&](int row, int col) {
        if (!grid.inBounds(row, col)) return;
        switch (currentMode) {
            case PlaceMode::WALL:  grid.setWall(row, col); break;
            case PlaceMode::START: grid.setStart(row, col); break;
            case PlaceMode::END:   grid.setEnd(row, col); break;
            case PlaceMode::ERASE:
                Cell& cell = grid.getCell(row, col);
                if (!cell.isStart() && !cell.isEnd()) cell.terrain = Terrain::EMPTY;
                break;
        }
    };

    auto loadPreset = [&](Difficulty diff, int level) {
        bool success = MazeLoader::loadPreset(grid, diff, level);
        auto info = PresetMazes::getInfo(diff, level);
        currentDifficulty = diff;
        currentLevel = level;
        panel.setPresetInfo(MazeLoader::difficultyName(diff), level, info.name);

        std::cout << "\n>>> Loaded: " << MazeLoader::difficultyName(diff)
                  << " Level " << level << " - " << info.name << std::endl;
        std::cout << "    " << info.description << std::endl;
        std::cout << "    Path Validation: " << (success ? "PASSED" : "FAILED") << std::endl;
    };

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // ESC always quits
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }

            // ========================================
            // STARTUP SCREEN
            // ========================================
            if (appState == AppState::STARTUP) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Num1) {
                        gameMode = GameMode::CUSTOM;
                        appState = AppState::RUNNING;
                        panel.setGameMode("CUSTOM");
                        panel.setMode("WALL");
                        std::cout << "\n>>> CUSTOM MODE selected - Draw your maze!" << std::endl;
                    }
                    if (event.key.code == sf::Keyboard::Num2) {
                        gameMode = GameMode::PRESET;
                        appState = AppState::RUNNING;
                        panel.setGameMode("PRESET");
                        loadPreset(Difficulty::BEGINNER, 1);
                        std::cout << "\n>>> PRESET MODE - Use B/I/A + 1-5 to choose levels" << std::endl;
                    }
                }
                continue; // skip rest of event handling
            }

            // ========================================
            // RUNNING STATE
            // ========================================

            // Mouse events (only in CUSTOM mode)
            if (gameMode == GameMode::CUSTOM) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.x >= MAZE_WIDTH) continue;
                    int row, col;
                    mouseToGrid(event.mouseButton.x, event.mouseButton.y, row, col);

                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if (grid.inBounds(row, col)) {
                            if (currentMode == PlaceMode::WALL) grid.toggleWall(row, col);
                            else applyMode(row, col);
                            isDragging = true;
                        }
                    }
                    if (event.mouseButton.button == sf::Mouse::Right) {
                        if (grid.inBounds(row, col)) grid.setStart(row, col);
                    }
                    if (event.mouseButton.button == sf::Mouse::Middle) {
                        if (grid.inBounds(row, col)) grid.setEnd(row, col);
                    }
                }

                if (event.type == sf::Event::MouseButtonReleased) {
                    if (event.mouseButton.button == sf::Mouse::Left) isDragging = false;
                }
            }

            // Hover always tracked
            if (event.type == sf::Event::MouseMoved) {
                int row, col;
                mouseToGrid(event.mouseMove.x, event.mouseMove.y, row, col);
                if (event.mouseMove.x < MAZE_WIDTH && grid.inBounds(row, col)) {
                    renderer.setHover(row, col);
                    if (isDragging && gameMode == GameMode::CUSTOM) {
                        if (currentMode == PlaceMode::WALL) grid.setWall(row, col);
                        else applyMode(row, col);
                    }
                } else {
                    renderer.clearHover();
                }
            }

            // Keyboard
            if (event.type == sf::Event::KeyPressed) {
                // Toggle Game Mode with M
                if (event.key.code == sf::Keyboard::M) {
                    if (gameMode == GameMode::CUSTOM) {
                        gameMode = GameMode::PRESET;
                        panel.setGameMode("PRESET");
                        loadPreset(Difficulty::BEGINNER, 1);
                    } else {
                        gameMode = GameMode::CUSTOM;
                        panel.setGameMode("CUSTOM");
                        panel.setPresetInfo("", 0, "");
                        grid.clearAll();
                        std::cout << "\n>>> Switched to CUSTOM mode" << std::endl;
                    }
                }

                // Preset controls
                if (gameMode == GameMode::PRESET) {
                    if (event.key.code == sf::Keyboard::B) {
                        currentDifficulty = Difficulty::BEGINNER;
                        loadPreset(currentDifficulty, currentLevel);
                    }
                    if (event.key.code == sf::Keyboard::I) {
                        currentDifficulty = Difficulty::INTERMEDIATE;
                        loadPreset(currentDifficulty, currentLevel);
                    }
                    if (event.key.code == sf::Keyboard::A) {
                        currentDifficulty = Difficulty::ADVANCED;
                        loadPreset(currentDifficulty, currentLevel);
                    }
                    if (event.key.code == sf::Keyboard::Num1) loadPreset(currentDifficulty, 1);
                    if (event.key.code == sf::Keyboard::Num2) loadPreset(currentDifficulty, 2);
                    if (event.key.code == sf::Keyboard::Num3) loadPreset(currentDifficulty, 3);
                    if (event.key.code == sf::Keyboard::Num4) loadPreset(currentDifficulty, 4);
                    if (event.key.code == sf::Keyboard::Num5) loadPreset(currentDifficulty, 5);
                }

                // Custom placement modes
                if (gameMode == GameMode::CUSTOM) {
                    if (event.key.code == sf::Keyboard::W) { currentMode = PlaceMode::WALL; panel.setMode("WALL"); }
                    if (event.key.code == sf::Keyboard::S) { currentMode = PlaceMode::START; panel.setMode("START"); }
                    if (event.key.code == sf::Keyboard::E) { currentMode = PlaceMode::END; panel.setMode("END"); }
                    if (event.key.code == sf::Keyboard::X) { currentMode = PlaceMode::ERASE; panel.setMode("ERASE"); }
                }

                // Common commands
                if (event.key.code == sf::Keyboard::C) {
                    grid.clearAll();
                    if (gameMode == GameMode::PRESET) panel.setPresetInfo("", 0, "");
                }
                if (event.key.code == sf::Keyboard::G) {
                    grid.clearVisuals();
                }
            }
        }

        // Render
        window.clear(Colors::BG_DARK);
if (appState == AppState::STARTUP) {
    static sf::Clock animClock;
    startupScreen.update(animClock.restart().asSeconds());
    startupScreen.render(window);
} else {
    renderer.render(window, grid);
    panel.render(window);
}

        window.display();
    }

    return 0;
}