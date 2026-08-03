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

public:
    bool loadFont() {
        std::vector<std::string> fontPaths = {
            "assets/fonts/arial.ttf",
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/segoeui.ttf",
            "C:/Windows/Fonts/consola.ttf"
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
        drawText(window, "PATHFORGE", MAZE_WIDTH + 20, 15, 22, Colors::TEXT_PRIMARY, true);
        drawText(window, "Pathfinding Engine", MAZE_WIDTH + 20, 42, 11, Colors::TEXT_SECONDARY);

        drawDivider(window, 68);

        // Game Mode
        drawText(window, "GAME MODE", MAZE_WIDTH + 20, 78, 10, Colors::TEXT_SECONDARY);
        sf::RectangleShape gameModeBadge(sf::Vector2f(UI_PANEL_WIDTH - 40, 28));
        gameModeBadge.setPosition(MAZE_WIDTH + 20, 92);
        gameModeBadge.setFillColor(currentGameMode == "PRESET" ? sf::Color(76, 175, 80) : sf::Color(255, 152, 0));
        window.draw(gameModeBadge);
        drawText(window, currentGameMode, MAZE_WIDTH + 30, 96, 15, Colors::TEXT_PRIMARY, true);

        // Show preset info if in preset mode
        int yPos = 130;
        if (currentGameMode == "PRESET" && !currentPresetName.empty()) {
            drawText(window, currentDifficulty + "  LEVEL " + std::to_string(currentLevel),
                     MAZE_WIDTH + 20, yPos, 11, Colors::TEXT_ACCENT);
            drawText(window, currentPresetName, MAZE_WIDTH + 20, yPos + 15, 13, Colors::TEXT_PRIMARY, true);
            yPos += 45;
        }

        drawDivider(window, yPos);
        yPos += 10;

        // Current placement mode (only relevant for custom)
        drawText(window, "PLACEMENT MODE", MAZE_WIDTH + 20, yPos, 10, Colors::TEXT_SECONDARY);
        yPos += 14;
        sf::RectangleShape modeBadge(sf::Vector2f(UI_PANEL_WIDTH - 40, 28));
        modeBadge.setPosition(MAZE_WIDTH + 20, yPos);
        modeBadge.setFillColor(getModeColor());
        window.draw(modeBadge);
        drawText(window, currentMode, MAZE_WIDTH + 30, yPos + 4, 15, Colors::TEXT_PRIMARY, true);
        yPos += 40;

        drawDivider(window, yPos);
        yPos += 10;

        // Game mode switch
        drawText(window, "GAME MODE", MAZE_WIDTH + 20, yPos, 10, Colors::TEXT_SECONDARY);
        yPos += 15;
        drawKeyBinding(window, "M", "Toggle Custom/Preset", yPos); yPos += 24;

        drawDivider(window, yPos + 5);
        yPos += 15;

        // Preset selection
        drawText(window, "PRESETS (in Preset mode)", MAZE_WIDTH + 20, yPos, 10, Colors::TEXT_SECONDARY);
        yPos += 15;
        drawKeyBinding(window, "B", "Beginner", yPos); yPos += 24;
        drawKeyBinding(window, "I", "Intermediate", yPos); yPos += 24;
        drawKeyBinding(window, "A", "Advanced", yPos); yPos += 24;
        drawKeyBinding(window, "1-5", "Level 1-5", yPos); yPos += 24;

        drawDivider(window, yPos + 5);
        yPos += 15;

        // Placement keys
        drawText(window, "PLACEMENT (Custom mode)", MAZE_WIDTH + 20, yPos, 10, Colors::TEXT_SECONDARY);
        yPos += 15;
        drawKeyBinding(window, "W", "Wall", yPos); yPos += 24;
        drawKeyBinding(window, "S", "Start", yPos); yPos += 24;
        drawKeyBinding(window, "E", "End", yPos); yPos += 24;
        drawKeyBinding(window, "X", "Erase", yPos); yPos += 24;

        drawDivider(window, yPos + 5);
        yPos += 15;

        // Commands
        drawText(window, "COMMANDS", MAZE_WIDTH + 20, yPos, 10, Colors::TEXT_SECONDARY);
        yPos += 15;
        drawKeyBinding(window, "C", "Clear all", yPos); yPos += 24;
        drawKeyBinding(window, "G", "Clear paths", yPos); yPos += 24;

        // Footer
        drawText(window, "v1.5 | Part 1.5", MAZE_WIDTH + 20, WINDOW_HEIGHT - 25, 10, Colors::TEXT_SECONDARY);
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
                         const std::string& desc, float y) {
        sf::RectangleShape keyBadge(sf::Vector2f(38, 20));
        keyBadge.setPosition(MAZE_WIDTH + 20, y);
        keyBadge.setFillColor(Colors::BUTTON_IDLE);
        keyBadge.setOutlineThickness(1);
        keyBadge.setOutlineColor(Colors::BORDER_LIGHT);
        window.draw(keyBadge);

        drawText(window, key, MAZE_WIDTH + 25, y + 2, 12, Colors::TEXT_ACCENT, true);
        drawText(window, desc, MAZE_WIDTH + 68, y + 3, 12, Colors::TEXT_PRIMARY);
    }

    sf::Color getModeColor() {
        if (currentMode == "WALL")  return sf::Color(85, 95, 120);
        if (currentMode == "START") return sf::Color(46, 213, 115);
        if (currentMode == "END")   return sf::Color(255, 71, 87);
        if (currentMode == "ERASE") return sf::Color(120, 120, 130);
        return Colors::BUTTON_IDLE;
    }
};