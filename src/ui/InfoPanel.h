#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
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

    void render(sf::RenderWindow& window) {
        // Panel background
        sf::RectangleShape panel(sf::Vector2f(UI_PANEL_WIDTH, WINDOW_HEIGHT));
        panel.setPosition(MAZE_WIDTH, 0);
        panel.setFillColor(Colors::BG_MID);
        window.draw(panel);

        sf::RectangleShape border(sf::Vector2f(2, WINDOW_HEIGHT));
        border.setPosition(MAZE_WIDTH, 0);
        border.setFillColor(Colors::BORDER_LIGHT);
        window.draw(border);

        if (!fontLoaded) return;

        // Title
        drawText(window, "PATHFORGE", MAZE_WIDTH + 20, 12, 20, Colors::TEXT_PRIMARY, true);

        int yPos = 42;
        drawDivider(window, yPos); yPos += 8;

        // Game Mode
        drawText(window, "GAME MODE", MAZE_WIDTH + 20, yPos, 9, Colors::TEXT_SECONDARY);
        yPos += 12;
        sf::RectangleShape gm(sf::Vector2f(UI_PANEL_WIDTH - 40, 22));
        gm.setPosition(MAZE_WIDTH + 20, yPos);
        gm.setFillColor(currentGameMode == "PRESET" ? sf::Color(76, 175, 80) : sf::Color(255, 152, 0));
        window.draw(gm);
        drawText(window, currentGameMode, MAZE_WIDTH + 28, yPos + 3, 13, Colors::TEXT_PRIMARY, true);
        yPos += 30;

        // Preset info
        if (currentGameMode == "PRESET" && !currentPresetName.empty()) {
            drawText(window, currentDifficulty + " L" + std::to_string(currentLevel),
                     MAZE_WIDTH + 20, yPos, 10, Colors::TEXT_ACCENT);
            yPos += 12;
            drawText(window, currentPresetName, MAZE_WIDTH + 20, yPos, 12, Colors::TEXT_PRIMARY, true);
            yPos += 20;
        }

        drawDivider(window, yPos); yPos += 8;

        // ============================================
        // ALGORITHM STATS (when running)
        // ============================================
        if (!algo1Name.empty() || !algo2Name.empty()) {
            drawText(window, "ALGORITHM STATS", MAZE_WIDTH + 20, yPos, 9, Colors::TEXT_SECONDARY);
            yPos += 15;

            if (!algo1Name.empty()) {
                drawAlgoBox(window, yPos, algo1Name, algo1Nodes, algo1PathLen, algo1Found, algo1Running, getAlgoColor(algo1Name));
                yPos += 78;
            }
            if (!algo2Name.empty()) {
                drawAlgoBox(window, yPos, algo2Name, algo2Nodes, algo2PathLen, algo2Found, algo2Running, getAlgoColor(algo2Name));
                yPos += 78;
            }

            drawDivider(window, yPos); yPos += 8;
        }

        // Placement Mode (Custom only)
        if (currentGameMode == "CUSTOM") {
            drawText(window, "PLACEMENT", MAZE_WIDTH + 20, yPos, 9, Colors::TEXT_SECONDARY);
            yPos += 12;
            sf::RectangleShape mb(sf::Vector2f(UI_PANEL_WIDTH - 40, 22));
            mb.setPosition(MAZE_WIDTH + 20, yPos);
            mb.setFillColor(getModeColor());
            window.draw(mb);
            drawText(window, currentMode, MAZE_WIDTH + 28, yPos + 3, 13, Colors::TEXT_PRIMARY, true);
            yPos += 30;
            drawDivider(window, yPos); yPos += 8;
        }

// ============================================
// ALGORITHM KEYS (mode-aware)
// ============================================
drawText(window, "PATHFINDING", MAZE_WIDTH + 20, yPos, 9, Colors::TEXT_SECONDARY);
yPos += 13;

if (currentGameMode == "PRESET") {
    drawKeyBinding(window, "Q", "BFS", yPos, Colors::BFS_PATH); yPos += 20;
    drawKeyBinding(window, "E", "DFS", yPos, Colors::DFS_PATH); yPos += 20;
    drawKeyBinding(window, "T", "A*", yPos, Colors::ASTAR_PATH); yPos += 20;
    drawKeyBinding(window, "Y", "Dijkstra", yPos, Colors::DIJKSTRA_PATH); yPos += 20;
    drawKeyBinding(window, "U", "Greedy", yPos, Colors::GREEDY_PATH); yPos += 20;
    drawKeyBinding(window, "O", "BiDir BFS", yPos, Colors::BIDIR_PATH); yPos += 20;
    drawKeyBinding(window, "P", "IDDFS", yPos, sf::Color(180, 180, 200)); yPos += 20;
    drawText(window, "RACE MODES", MAZE_WIDTH + 20, yPos + 3, 9, Colors::TEXT_SECONDARY); yPos += 16;
    drawKeyBinding(window, "R", "BFS vs DFS", yPos, sf::Color(200,200,100)); yPos += 20;
    drawKeyBinding(window, "Z", "A* vs BFS", yPos, sf::Color(150,200,200)); yPos += 20;
} else {
    drawKeyBinding(window, "1", "BFS", yPos, Colors::BFS_PATH); yPos += 20;
    drawKeyBinding(window, "2", "DFS", yPos, Colors::DFS_PATH); yPos += 20;
    drawKeyBinding(window, "4", "A*", yPos, Colors::ASTAR_PATH); yPos += 20;
    drawKeyBinding(window, "5", "Dijkstra", yPos, Colors::DIJKSTRA_PATH); yPos += 20;
    drawKeyBinding(window, "6", "Greedy", yPos, Colors::GREEDY_PATH); yPos += 20;
    drawKeyBinding(window, "7", "BiDir BFS", yPos, Colors::BIDIR_PATH); yPos += 20;
    drawKeyBinding(window, "8", "IDDFS", yPos, sf::Color(180, 180, 200)); yPos += 20;
    drawText(window, "RACE MODES", MAZE_WIDTH + 20, yPos + 3, 9, Colors::TEXT_SECONDARY); yPos += 16;
    drawKeyBinding(window, "3", "BFS vs DFS", yPos, sf::Color(200,200,100)); yPos += 20;
    drawKeyBinding(window, "9", "A* vs BFS", yPos, sf::Color(150,200,200)); yPos += 20;
}

        drawDivider(window, yPos + 3); yPos += 12;

        // Speed
        drawText(window, "SPEED: " + std::to_string(animationSpeed) + "ms", MAZE_WIDTH + 20, yPos, 10, Colors::TEXT_ACCENT);
        yPos += 15;
        drawKeyBinding(window, "+", "Faster", yPos); yPos += 22;
        drawKeyBinding(window, "-", "Slower", yPos); yPos += 22;

        drawDivider(window, yPos + 3); yPos += 12;

        // Mode switching
        if (currentGameMode == "PRESET") {
            drawText(window, "DIFFICULTY", MAZE_WIDTH + 20, yPos, 9, Colors::TEXT_SECONDARY);
             yPos += 13;
             drawKeyBinding(window, "B", "Beginner", yPos); yPos += 22;
             drawKeyBinding(window, "I", "Intermediate", yPos); yPos += 22;
             drawKeyBinding(window, "A", "Advanced", yPos); yPos += 22;

            drawText(window, "LEVELS", MAZE_WIDTH + 20, yPos + 3, 9, Colors::TEXT_SECONDARY);
            yPos += 16;
            drawKeyBinding(window, "1-5", "Load level", yPos); yPos += 22;
        } else {
            drawText(window, "PLACEMENT KEYS", MAZE_WIDTH + 20, yPos, 9, Colors::TEXT_SECONDARY);
            yPos += 13;
            drawKeyBinding(window, "W", "Wall", yPos); yPos += 22;
            drawKeyBinding(window, "S", "Start", yPos); yPos += 22;
            drawKeyBinding(window, "E", "End", yPos); yPos += 22;
        }

        // Commands (always at bottom)
        drawText(window, "COMMANDS", MAZE_WIDTH + 20, WINDOW_HEIGHT - 90, 9, Colors::TEXT_SECONDARY);
        drawKeyBinding(window, "G", "Clear paths", WINDOW_HEIGHT - 75);
        drawKeyBinding(window, "C", "Clear all", WINDOW_HEIGHT - 53);
        drawKeyBinding(window, "M", "Switch mode", WINDOW_HEIGHT - 31);

        // Version
        drawText(window, "v2.0", MAZE_WIDTH + UI_PANEL_WIDTH - 40, WINDOW_HEIGHT - 15, 9, Colors::TEXT_SECONDARY);
    }

private:
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

    // Key badge with DARK background (always readable)
    sf::RectangleShape kb(sf::Vector2f(32, 18));
    kb.setPosition(MAZE_WIDTH + 20, y);
    kb.setFillColor(Colors::BG_DARK);   // dark background
    kb.setOutlineThickness(1.5f);
    kb.setOutlineColor(accent.a > 0 ? accent : Colors::BORDER_LIGHT);  // colored border
    window.draw(kb);

    // Colored accent strip on left of key (like a colored tab)
    if (accent.a > 0) {
        sf::RectangleShape strip(sf::Vector2f(3, 18));
        strip.setPosition(MAZE_WIDTH + 20, y);
        strip.setFillColor(accent);
        window.draw(strip);
    }

    // Key letter - always bright white/accent color
    sf::Color keyTextColor = accent.a > 0 ? accent : Colors::TEXT_PRIMARY;
    drawText(window, key, MAZE_WIDTH + 28, y + 1, 12, keyTextColor, true);

    // Description
    drawText(window, desc, MAZE_WIDTH + 60, y + 2, 12, Colors::TEXT_PRIMARY);
}

    void drawAlgoBox(sf::RenderWindow& window, float y, const std::string& name,
                      int nodes, int pathLen, bool found, bool running, sf::Color color) {
        // Box
        sf::RectangleShape box(sf::Vector2f(UI_PANEL_WIDTH - 40, 70));
        box.setPosition(MAZE_WIDTH + 20, y);
        box.setFillColor(Colors::BG_DARK);
        box.setOutlineThickness(1);
        box.setOutlineColor(color);
        window.draw(box);

        // Color strip on left
        sf::RectangleShape strip(sf::Vector2f(4, 70));
        strip.setPosition(MAZE_WIDTH + 20, y);
        strip.setFillColor(color);
        window.draw(strip);

        // Name + status
        drawText(window, name, MAZE_WIDTH + 32, y + 6, 14, Colors::TEXT_PRIMARY, true);

        std::string status = running ? "RUNNING..." : (found ? "PATH FOUND" : "NO PATH");
        sf::Color statusCol = running ? Colors::TEXT_ACCENT : (found ? Colors::START : Colors::END);
        drawText(window, status, MAZE_WIDTH + UI_PANEL_WIDTH - 100, y + 8, 10, statusCol, true);

        // Stats
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
        return Colors::BUTTON_IDLE;
    }

    sf::Color getAlgoColor(const std::string& name) {
        if (name == "BFS") return Colors::BFS_PATH;
        if (name == "DFS") return Colors::DFS_PATH;
        return Colors::TEXT_ACCENT;
    }
};