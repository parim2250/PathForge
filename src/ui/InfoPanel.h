#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include "../rendering/Colors.h"
#include "../core/Constants.h"

class InfoPanel {
private:
    sf::Font font;
    bool fontLoaded = false;
    std::string currentMode = "WALL";
    std::string currentGameMode = "CUSTOM";
    std::string currentPresetName = "";
    std::string currentDifficulty = "";
    int currentLevel = 0;

    // Algorithm stats
    std::string algo1Name = "";
    int algo1Nodes = 0;
    int algo1PathLen = 0;
    bool algo1Found = false;
    bool algo1Running = false;

    std::string algo2Name = "";
    int algo2Nodes = 0;
    int algo2PathLen = 0;
    bool algo2Found = false;
    bool algo2Running = false;

    int animationSpeed = 20;

    // ============================================
    // SCROLLING
    // ============================================
    float scrollOffset = 0;
    float contentHeight = 0;
    float maxScroll = 0;
    const float SCROLL_SPEED = 30.0f;

public:
    bool loadFont() {
        std::vector<std::string> fontPaths = {
            "assets/fonts/arial.ttf",
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/segoeui.ttf"
        };
        for (const auto& path : fontPaths) {
            if (font.loadFromFile(path)) {
                fontLoaded = true;
                return true;
            }
        }
        return false;
    }

    void setMode(const std::string& mode) { currentMode = mode; }
    void setGameMode(const std::string& mode) { currentGameMode = mode; }
    void setPresetInfo(const std::string& difficulty, int level, const std::string& name) {
        currentDifficulty = difficulty;
        currentLevel = level;
        currentPresetName = name;
    }

    void setAlgo1(const std::string& name, int nodes, int pathLen, bool found, bool running) {
        algo1Name = name; algo1Nodes = nodes; algo1PathLen = pathLen;
        algo1Found = found; algo1Running = running;
    }

    void setAlgo2(const std::string& name, int nodes, int pathLen, bool found, bool running) {
        algo2Name = name; algo2Nodes = nodes; algo2PathLen = pathLen;
        algo2Found = found; algo2Running = running;
    }

    void clearAlgoStats() {
        algo1Name = ""; algo2Name = "";
        algo1Running = false; algo2Running = false;
    }

    void setSpeed(int speed) { animationSpeed = speed; }

    // ============================================
    // SCROLL HANDLING (call from main.cpp on mouse wheel)
    // ============================================
    void scroll(float delta) {
        scrollOffset -= delta * SCROLL_SPEED;
        if (scrollOffset < 0) scrollOffset = 0;
        if (scrollOffset > maxScroll) scrollOffset = maxScroll;
    }

    // ============================================
    // MAIN RENDER
    // ============================================
    void render(sf::RenderWindow& window) {
        // Solid panel background (always visible)
        sf::RectangleShape panel(sf::Vector2f(UI_PANEL_WIDTH, WINDOW_HEIGHT));
        panel.setPosition(MAZE_WIDTH, 0);
        panel.setFillColor(Colors::BG_MID);
        window.draw(panel);

        sf::RectangleShape border(sf::Vector2f(2, WINDOW_HEIGHT));
        border.setPosition(MAZE_WIDTH, 0);
        border.setFillColor(Colors::BORDER_LIGHT);
        window.draw(border);

        if (!fontLoaded) return;

        // ============================================
        // FIXED HEADER (does NOT scroll)
        // ============================================
        drawText(window, "PATHFORGE", MAZE_WIDTH + 20, 12, 20, Colors::TEXT_PRIMARY, true);
        drawDivider(window, 42);

        // Set up clipping/view for scrollable content
        // Header ends at y=50, footer starts at WINDOW_HEIGHT-70
        float scrollAreaStart = 50;
        float scrollAreaEnd = WINDOW_HEIGHT - 70;

        // Render scrollable content
        float yPos = scrollAreaStart - scrollOffset;
        float startY = yPos;

        // ============================================
        // GAME MODE
        // ============================================
        if (yPos > 30 && yPos < scrollAreaEnd) {
            drawText(window, "GAME MODE", MAZE_WIDTH + 20, yPos, 9, Colors::TEXT_SECONDARY);
        }
        yPos += 12;
        if (yPos > 30 && yPos < scrollAreaEnd) {
            sf::RectangleShape gm(sf::Vector2f(UI_PANEL_WIDTH - 40, 22));
            gm.setPosition(MAZE_WIDTH + 20, yPos);
            gm.setFillColor(currentGameMode == "PRESET" ? sf::Color(76, 175, 80) : sf::Color(255, 152, 0));
            window.draw(gm);
            drawText(window, currentGameMode, MAZE_WIDTH + 28, yPos + 3, 13, Colors::TEXT_PRIMARY, true);
        }
        yPos += 30;

        // Preset info
        if (currentGameMode == "PRESET" && !currentPresetName.empty()) {
            if (yPos > 30 && yPos < scrollAreaEnd) {
                drawText(window, currentDifficulty + " L" + std::to_string(currentLevel),
                         MAZE_WIDTH + 20, yPos, 10, Colors::TEXT_ACCENT);
            }
            yPos += 12;
            if (yPos > 30 && yPos < scrollAreaEnd) {
                drawText(window, currentPresetName, MAZE_WIDTH + 20, yPos, 12, Colors::TEXT_PRIMARY, true);
            }
            yPos += 20;
        }

        if (yPos > 30 && yPos < scrollAreaEnd) drawDivider(window, yPos);
        yPos += 8;

        // ============================================
        // ALGORITHM STATS
        // ============================================
        if (!algo1Name.empty() || !algo2Name.empty()) {
            if (yPos > 30 && yPos < scrollAreaEnd) {
                drawText(window, "ALGORITHM STATS", MAZE_WIDTH + 20, yPos, 9, Colors::TEXT_SECONDARY);
            }
            yPos += 15;

            if (!algo1Name.empty()) {
                if (yPos > 30 && yPos < scrollAreaEnd) {
                    drawAlgoBox(window, yPos, algo1Name, algo1Nodes, algo1PathLen, algo1Found, algo1Running, getAlgoColor(algo1Name));
                }
                yPos += 78;
            }
            if (!algo2Name.empty()) {
                if (yPos > 30 && yPos < scrollAreaEnd) {
                    drawAlgoBox(window, yPos, algo2Name, algo2Nodes, algo2PathLen, algo2Found, algo2Running, getAlgoColor(algo2Name));
                }
                yPos += 78;
            }

            if (yPos > 30 && yPos < scrollAreaEnd) drawDivider(window, yPos);
            yPos += 8;
        }

        // ============================================
        // PLACEMENT MODE (Custom only)
        // ============================================
        if (currentGameMode == "CUSTOM") {
            if (yPos > 30 && yPos < scrollAreaEnd) {
                drawText(window, "CURRENT TOOL", MAZE_WIDTH + 20, yPos, 9, Colors::TEXT_SECONDARY);
            }
            yPos += 12;
            if (yPos > 30 && yPos < scrollAreaEnd) {
                sf::RectangleShape mb(sf::Vector2f(UI_PANEL_WIDTH - 40, 22));
                mb.setPosition(MAZE_WIDTH + 20, yPos);
                mb.setFillColor(getModeColor());
                window.draw(mb);
                drawText(window, currentMode, MAZE_WIDTH + 28, yPos + 3, 13, Colors::TEXT_PRIMARY, true);
            }
            yPos += 30;
            if (yPos > 30 && yPos < scrollAreaEnd) drawDivider(window, yPos);
            yPos += 8;
        }

        // ============================================
        // ALGORITHM KEYS
        // ============================================
        if (yPos > 30 && yPos < scrollAreaEnd) {
            drawText(window, "PATHFINDING", MAZE_WIDTH + 20, yPos, 9, Colors::TEXT_SECONDARY);
        }
        yPos += 13;

        if (currentGameMode == "PRESET") {
            drawKeyIfVisible(window, "Q", "BFS", yPos, Colors::BFS_PATH, scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "E", "DFS", yPos, Colors::DFS_PATH, scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "T", "A*", yPos, Colors::ASTAR_PATH, scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "Y", "Dijkstra", yPos, Colors::DIJKSTRA_PATH, scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "U", "Greedy", yPos, Colors::GREEDY_PATH, scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "O", "BiDir BFS", yPos, Colors::BIDIR_PATH, scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "P", "IDDFS", yPos, sf::Color(180, 180, 200), scrollAreaEnd); yPos += 20;

            if (yPos > 30 && yPos < scrollAreaEnd) {
                drawText(window, "RACE MODES", MAZE_WIDTH + 20, yPos + 3, 9, Colors::TEXT_SECONDARY);
            }
            yPos += 16;
            drawKeyIfVisible(window, "R", "BFS vs DFS", yPos, sf::Color(200,200,100), scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "Z", "A* vs BFS", yPos, sf::Color(150,200,200), scrollAreaEnd); yPos += 20;
        } else {
            drawKeyIfVisible(window, "1", "BFS", yPos, Colors::BFS_PATH, scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "2", "DFS", yPos, Colors::DFS_PATH, scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "4", "A*", yPos, Colors::ASTAR_PATH, scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "5", "Dijkstra", yPos, Colors::DIJKSTRA_PATH, scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "6", "Greedy", yPos, Colors::GREEDY_PATH, scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "7", "BiDir BFS", yPos, Colors::BIDIR_PATH, scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "8", "IDDFS", yPos, sf::Color(180, 180, 200), scrollAreaEnd); yPos += 20;

            if (yPos > 30 && yPos < scrollAreaEnd) {
                drawText(window, "RACE MODES", MAZE_WIDTH + 20, yPos + 3, 9, Colors::TEXT_SECONDARY);
            }
            yPos += 16;
            drawKeyIfVisible(window, "3", "BFS vs DFS", yPos, sf::Color(200,200,100), scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "9", "A* vs BFS", yPos, sf::Color(150,200,200), scrollAreaEnd); yPos += 20;
        }

        if (yPos > 30 && yPos < scrollAreaEnd) drawDivider(window, yPos + 3);
        yPos += 12;

        // ============================================
        // SPEED CONTROL
        // ============================================
        if (yPos > 30 && yPos < scrollAreaEnd) {
            drawText(window, "SPEED: " + std::to_string(animationSpeed) + "ms",
                     MAZE_WIDTH + 20, yPos, 10, Colors::TEXT_ACCENT);
        }
        yPos += 15;
        drawKeyIfVisible(window, "+", "Faster", yPos, sf::Color(100, 220, 100), scrollAreaEnd); yPos += 20;
        drawKeyIfVisible(window, "-", "Slower", yPos, sf::Color(220, 100, 100), scrollAreaEnd); yPos += 20;

        if (yPos > 30 && yPos < scrollAreaEnd) drawDivider(window, yPos + 3);
        yPos += 12;

        // ============================================
        // MODE-SPECIFIC KEYS
        // ============================================
        if (currentGameMode == "PRESET") {
            if (yPos > 30 && yPos < scrollAreaEnd) {
                drawText(window, "DIFFICULTY", MAZE_WIDTH + 20, yPos, 9, Colors::TEXT_SECONDARY);
            }
            yPos += 13;
            drawKeyIfVisible(window, "B", "Beginner", yPos, sf::Color(100, 200, 100), scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "I", "Intermediate", yPos, sf::Color(255, 180, 100), scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "A", "Advanced", yPos, sf::Color(255, 100, 100), scrollAreaEnd); yPos += 20;

            if (yPos > 30 && yPos < scrollAreaEnd) {
                drawText(window, "LEVELS", MAZE_WIDTH + 20, yPos + 3, 9, Colors::TEXT_SECONDARY);
            }
            yPos += 16;
            drawKeyIfVisible(window, "1-5", "Load level", yPos, Colors::TEXT_ACCENT, scrollAreaEnd); yPos += 20;

            // Terrain mazes highlight
            if (currentDifficulty == "INTERMEDIATE") {
                if (yPos > 30 && yPos < scrollAreaEnd) {
                    drawText(window, "TERRAIN MAZES", MAZE_WIDTH + 20, yPos + 3, 9, Colors::TEXT_SECONDARY);
                }
                yPos += 16;
                drawKeyIfVisible(window, "3", "River Crossing", yPos, sf::Color(84, 160, 255), scrollAreaEnd); yPos += 20;
                drawKeyIfVisible(window, "4", "Sandy Desert", yPos, sf::Color(255, 195, 105), scrollAreaEnd); yPos += 20;
                drawKeyIfVisible(window, "5", "Muddy Swamp", yPos, sf::Color(139, 90, 60), scrollAreaEnd); yPos += 20;
            }
        } else {
            if (yPos > 30 && yPos < scrollAreaEnd) {
                drawText(window, "PLACEMENT", MAZE_WIDTH + 20, yPos, 9, Colors::TEXT_SECONDARY);
            }
            yPos += 13;
            drawKeyIfVisible(window, "W", "Wall", yPos, sf::Color(85, 95, 120), scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "S", "Start", yPos, sf::Color(46, 213, 115), scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "E", "End", yPos, sf::Color(255, 71, 87), scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "X", "Erase", yPos, sf::Color(120, 120, 130), scrollAreaEnd); yPos += 20;

            if (yPos > 30 && yPos < scrollAreaEnd) {
                drawText(window, "TERRAIN", MAZE_WIDTH + 20, yPos + 3, 9, Colors::TEXT_SECONDARY);
            }
            yPos += 16;
            drawKeyIfVisible(window, "F", "Sand (3x)", yPos, sf::Color(255, 195, 105), scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "V", "Water (5x)", yPos, sf::Color(84, 160, 255), scrollAreaEnd); yPos += 20;
            drawKeyIfVisible(window, "D", "Mud (8x)", yPos, sf::Color(139, 90, 60), scrollAreaEnd); yPos += 20;
        }

        if (yPos > 30 && yPos < scrollAreaEnd) drawDivider(window, yPos + 3);
        yPos += 12;

        // ============================================
        // COMMANDS
        // ============================================
        if (yPos > 30 && yPos < scrollAreaEnd) {
            drawText(window, "COMMANDS", MAZE_WIDTH + 20, yPos, 9, Colors::TEXT_SECONDARY);
        }
        yPos += 13;
        drawKeyIfVisible(window, "G", "Clear paths", yPos, sf::Color(150, 200, 220), scrollAreaEnd); yPos += 20;
        drawKeyIfVisible(window, "C", "Clear all", yPos, sf::Color(220, 150, 150), scrollAreaEnd); yPos += 20;
        drawKeyIfVisible(window, "M", "Switch mode", yPos, sf::Color(200, 200, 100), scrollAreaEnd); yPos += 20;

        // Calculate max scroll based on content size
        contentHeight = yPos + scrollOffset - startY;
        float availableHeight = scrollAreaEnd - scrollAreaStart;
        maxScroll = std::max(0.0f, contentHeight - availableHeight + 20);

        // ============================================
        // MASK: Cover overflow above header (so text doesn't leak)
        // ============================================
        sf::RectangleShape topMask(sf::Vector2f(UI_PANEL_WIDTH - 2, 50));
        topMask.setPosition(MAZE_WIDTH + 2, 0);
        topMask.setFillColor(Colors::BG_MID);
        window.draw(topMask);

        // Redraw header on top
        drawText(window, "PATHFORGE", MAZE_WIDTH + 20, 12, 20, Colors::TEXT_PRIMARY, true);
        drawDivider(window, 42);

        // Bottom mask (so scrolled content doesn't overlap footer)
        sf::RectangleShape bottomMask(sf::Vector2f(UI_PANEL_WIDTH - 2, 80));
        bottomMask.setPosition(MAZE_WIDTH + 2, WINDOW_HEIGHT - 70);
        bottomMask.setFillColor(Colors::BG_MID);
        window.draw(bottomMask);

        // ============================================
        // FOOTER (fixed at bottom)
        // ============================================
        drawDivider(window, WINDOW_HEIGHT - 65);
        drawText(window, "SCROLL for more", MAZE_WIDTH + 20, WINDOW_HEIGHT - 55, 10, Colors::TEXT_SECONDARY);
        drawText(window, "Mouse wheel to scroll", MAZE_WIDTH + 20, WINDOW_HEIGHT - 40, 10, Colors::TEXT_SECONDARY);
        drawText(window, "v4.0 | PathForge", MAZE_WIDTH + 20, WINDOW_HEIGHT - 22, 10, Colors::TEXT_SECONDARY);

        // ============================================
        // SCROLL INDICATOR (right side of panel)
        // ============================================
        if (maxScroll > 0) {
            float trackHeight = scrollAreaEnd - scrollAreaStart;
            sf::RectangleShape track(sf::Vector2f(4, trackHeight));
            track.setPosition(MAZE_WIDTH + UI_PANEL_WIDTH - 8, scrollAreaStart);
            track.setFillColor(sf::Color(50, 55, 70));
            window.draw(track);

            float thumbHeight = std::max(30.0f, trackHeight * (trackHeight / contentHeight));
            float thumbY = scrollAreaStart + (scrollOffset / maxScroll) * (trackHeight - thumbHeight);

            sf::RectangleShape thumb(sf::Vector2f(4, thumbHeight));
            thumb.setPosition(MAZE_WIDTH + UI_PANEL_WIDTH - 8, thumbY);
            thumb.setFillColor(Colors::TEXT_ACCENT);
            window.draw(thumb);
        }
    }

private:
    // Helper: draw key only if visible in scroll area
    void drawKeyIfVisible(sf::RenderWindow& window, const std::string& key,
                           const std::string& desc, float y, sf::Color accent, float scrollEnd) {
        if (y > 30 && y < scrollEnd) {
            drawKeyBinding(window, key, desc, y, accent);
        }
    }

    void drawText(sf::RenderWindow& window, const std::string& str,
                  float x, float y, int size, sf::Color color, bool bold = false) {
        sf::Text text;
        text.setFont(font);
        text.setString(str);
        text.setCharacterSize(size);
        text.setFillColor(color);
        text.setPosition(x, y);
        if (bold) text.setStyle(sf::Text::Bold);
        window.draw(text);
    }

    void drawDivider(sf::RenderWindow& window, float y) {
        sf::RectangleShape line(sf::Vector2f(UI_PANEL_WIDTH - 40, 1));
        line.setPosition(MAZE_WIDTH + 20, y);
        line.setFillColor(Colors::BORDER);
        window.draw(line);
    }

    void drawKeyBinding(sf::RenderWindow& window, const std::string& key,
                         const std::string& desc, float y, sf::Color accent = sf::Color(0,0,0,0)) {
        sf::RectangleShape kb(sf::Vector2f(32, 18));
        kb.setPosition(MAZE_WIDTH + 20, y);
        kb.setFillColor(Colors::BG_DARK);
        kb.setOutlineThickness(1.5f);
        kb.setOutlineColor(accent.a > 0 ? accent : Colors::BORDER_LIGHT);
        window.draw(kb);

        if (accent.a > 0) {
            sf::RectangleShape strip(sf::Vector2f(3, 18));
            strip.setPosition(MAZE_WIDTH + 20, y);
            strip.setFillColor(accent);
            window.draw(strip);
        }

        sf::Color keyTextColor = accent.a > 0 ? accent : Colors::TEXT_PRIMARY;
        drawText(window, key, MAZE_WIDTH + 28, y + 1, 12, keyTextColor, true);
        drawText(window, desc, MAZE_WIDTH + 60, y + 2, 12, Colors::TEXT_PRIMARY);
    }

    void drawAlgoBox(sf::RenderWindow& window, float y, const std::string& name,
                      int nodes, int pathLen, bool found, bool running, sf::Color color) {
        sf::RectangleShape box(sf::Vector2f(UI_PANEL_WIDTH - 40, 70));
        box.setPosition(MAZE_WIDTH + 20, y);
        box.setFillColor(Colors::BG_DARK);
        box.setOutlineThickness(1);
        box.setOutlineColor(color);
        window.draw(box);

        sf::RectangleShape strip(sf::Vector2f(4, 70));
        strip.setPosition(MAZE_WIDTH + 20, y);
        strip.setFillColor(color);
        window.draw(strip);

        drawText(window, name, MAZE_WIDTH + 32, y + 6, 14, Colors::TEXT_PRIMARY, true);

        std::string status = running ? "RUNNING..." : (found ? "PATH FOUND" : "NO PATH");
        sf::Color statusCol = running ? Colors::TEXT_ACCENT : (found ? Colors::START : Colors::END);
        drawText(window, status, MAZE_WIDTH + UI_PANEL_WIDTH - 100, y + 8, 10, statusCol, true);

        drawText(window, "Nodes:", MAZE_WIDTH + 32, y + 28, 11, Colors::TEXT_SECONDARY);
        drawText(window, std::to_string(nodes), MAZE_WIDTH + 90, y + 28, 11, Colors::TEXT_PRIMARY, true);

        drawText(window, "Path:", MAZE_WIDTH + 32, y + 46, 11, Colors::TEXT_SECONDARY);
        drawText(window, std::to_string(pathLen) + " cells", MAZE_WIDTH + 90, y + 46, 11, Colors::TEXT_PRIMARY, true);
    }

    sf::Color getModeColor() {
        if (currentMode == "WALL")  return sf::Color(85, 95, 120);
        if (currentMode == "START") return sf::Color(46, 213, 115);
        if (currentMode == "END")   return sf::Color(255, 71, 87);
        if (currentMode == "ERASE") return sf::Color(120, 120, 130);
        if (currentMode == "SAND")  return sf::Color(255, 195, 105);
        if (currentMode == "WATER") return sf::Color(84, 160, 255);
        if (currentMode == "MUD")   return sf::Color(139, 90, 60);
        return Colors::BUTTON_IDLE;
    }

    sf::Color getAlgoColor(const std::string& name) {
        if (name == "BFS")      return Colors::BFS_PATH;
        if (name == "DFS")      return Colors::DFS_PATH;
        if (name == "A*")       return Colors::ASTAR_PATH;
        if (name == "Dijkstra") return Colors::DIJKSTRA_PATH;
        if (name == "Greedy")   return Colors::GREEDY_PATH;
        if (name == "BiDir BFS") return Colors::BIDIR_PATH;
        return Colors::TEXT_ACCENT;
    }
};