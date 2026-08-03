#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "../rendering/Colors.h"
#include "../core/Constants.h"

class InfoPanel {
private:
    sf::Font font;
    bool fontLoaded = false;
    std::string currentMode = "WALL";

public:
    // ============================================
    // LOAD FONT (try common Windows fonts)
    // ============================================
    bool loadFont() {
        // Try common paths for fonts
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

    // ============================================
    // SET CURRENT MODE
    // ============================================
    void setMode(const std::string& mode) {
        currentMode = mode;
    }

    // ============================================
    // RENDER THE ENTIRE PANEL
    // ============================================
    void render(sf::RenderWindow& window) {
        // Panel background
        sf::RectangleShape panel(sf::Vector2f(UI_PANEL_WIDTH, WINDOW_HEIGHT));
        panel.setPosition(MAZE_WIDTH, 0);
        panel.setFillColor(Colors::BG_MID);
        window.draw(panel);

        // Left border (accent line)
        sf::RectangleShape border(sf::Vector2f(2, WINDOW_HEIGHT));
        border.setPosition(MAZE_WIDTH, 0);
        border.setFillColor(Colors::BORDER_LIGHT);
        window.draw(border);

        if (!fontLoaded) return;

        // ============================================
        // TITLE
        // ============================================
        drawText(window, "PATHFORGE", MAZE_WIDTH + 20, 25, 24, Colors::TEXT_PRIMARY, true);
        drawText(window, "Pathfinding Engine", MAZE_WIDTH + 20, 55, 12, Colors::TEXT_SECONDARY);

        // Divider
        drawDivider(window, 90);

        // ============================================
        // CURRENT MODE
        // ============================================
        drawText(window, "CURRENT MODE", MAZE_WIDTH + 20, 105, 11, Colors::TEXT_SECONDARY);

        // Mode badge with color
        sf::Color modeColor = getModeColor();
        sf::RectangleShape modeBadge(sf::Vector2f(UI_PANEL_WIDTH - 40, 40));
        modeBadge.setPosition(MAZE_WIDTH + 20, 125);
        modeBadge.setFillColor(modeColor);
        window.draw(modeBadge);

        drawText(window, currentMode, MAZE_WIDTH + 35, 135, 18, Colors::TEXT_PRIMARY, true);

        // Divider
        drawDivider(window, 185);

        // ============================================
        // MODE CONTROLS
        // ============================================
        drawText(window, "MODE KEYS", MAZE_WIDTH + 20, 200, 11, Colors::TEXT_SECONDARY);

        int y = 225;
        drawKeyBinding(window, "W", "Wall mode", y);       y += 28;
        drawKeyBinding(window, "S", "Start point", y);     y += 28;
        drawKeyBinding(window, "E", "End point", y);       y += 28;
        drawKeyBinding(window, "X", "Erase mode", y);      y += 28;

        // Divider
        drawDivider(window, y + 10);

        // ============================================
        // MOUSE CONTROLS
        // ============================================
        y += 25;
        drawText(window, "MOUSE", MAZE_WIDTH + 20, y, 11, Colors::TEXT_SECONDARY);
        y += 25;

        drawText(window, "Left Click", MAZE_WIDTH + 20, y, 13, Colors::TEXT_PRIMARY);
        drawText(window, "Apply mode", MAZE_WIDTH + 130, y, 13, Colors::TEXT_SECONDARY);
        y += 22;

        drawText(window, "Left Drag", MAZE_WIDTH + 20, y, 13, Colors::TEXT_PRIMARY);
        drawText(window, "Paint walls", MAZE_WIDTH + 130, y, 13, Colors::TEXT_SECONDARY);
        y += 22;

        drawText(window, "Right Click", MAZE_WIDTH + 20, y, 13, Colors::TEXT_PRIMARY);
        drawText(window, "Quick start", MAZE_WIDTH + 130, y, 13, Colors::TEXT_SECONDARY);
        y += 22;

        // Divider
        drawDivider(window, y + 15);

        // ============================================
        // COMMANDS
        // ============================================
        y += 30;
        drawText(window, "COMMANDS", MAZE_WIDTH + 20, y, 11, Colors::TEXT_SECONDARY);
        y += 25;

        drawKeyBinding(window, "C", "Clear all", y);       y += 28;
        drawKeyBinding(window, "G", "Clear paths", y);     y += 28;

        // ============================================
        // FOOTER
        // ============================================
        drawText(window, "v1.0 | Part 1", MAZE_WIDTH + 20, WINDOW_HEIGHT - 30, 10, Colors::TEXT_SECONDARY);
    }

private:
    // ============================================
    // DRAW TEXT HELPER
    // ============================================
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

    // ============================================
    // DRAW A HORIZONTAL DIVIDER LINE
    // ============================================
    void drawDivider(sf::RenderWindow& window, float y) {
        sf::RectangleShape line(sf::Vector2f(UI_PANEL_WIDTH - 40, 1));
        line.setPosition(MAZE_WIDTH + 20, y);
        line.setFillColor(Colors::BORDER);
        window.draw(line);
    }

    // ============================================
    // DRAW KEY BINDING (e.g., "W  Wall mode")
    // ============================================
    void drawKeyBinding(sf::RenderWindow& window, const std::string& key,
                         const std::string& desc, float y) {
        // Key badge
        sf::RectangleShape keyBadge(sf::Vector2f(30, 22));
        keyBadge.setPosition(MAZE_WIDTH + 20, y);
        keyBadge.setFillColor(Colors::BUTTON_IDLE);
        keyBadge.setOutlineThickness(1);
        keyBadge.setOutlineColor(Colors::BORDER_LIGHT);
        window.draw(keyBadge);

        // Key letter
        drawText(window, key, MAZE_WIDTH + 30, y + 2, 14, Colors::TEXT_ACCENT, true);

        // Description
        drawText(window, desc, MAZE_WIDTH + 60, y + 3, 13, Colors::TEXT_PRIMARY);
    }

    // ============================================
    // GET COLOR BASED ON CURRENT MODE
    // ============================================
    sf::Color getModeColor() {
        if (currentMode == "WALL")  return sf::Color(85, 95, 120);
        if (currentMode == "START") return sf::Color(46, 213, 115);
        if (currentMode == "END")   return sf::Color(255, 71, 87);
        if (currentMode == "ERASE") return sf::Color(120, 120, 130);
        return Colors::BUTTON_IDLE;
    }
};