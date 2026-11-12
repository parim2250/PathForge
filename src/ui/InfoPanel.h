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

    int mouseX = 0, mouseY = 0;
    float startX = (float)MAZE_WIDTH;

    std::string selectedAlgo = "A*";
    std::string selectedTool = "Wall";
    bool isRunning = false;
    bool isPaused = false;
    int speedMs = 30;

    bool dropdownOpen = false;
    std::vector<std::string> algos = {"BFS", "DFS", "A*", "Dijkstra", "Greedy", "BiDir BFS", "IDDFS"};
    std::vector<std::string> tools = {"Wall", "Erase", "Start", "End", "Sand", "Water", "Mud"};

    int m_nodes = 0, m_pathLen = 0, m_cost = 0;
    long long m_time = 0;

    // Layout constants (render + hit-test MUST use the same values)
    static constexpr float PAD = 20.f;
    static constexpr float DROP_Y = 60.f;
    static constexpr float DROP_H = 36.f;
    static constexpr float CTRL_Y = 160.f;
    static constexpr float CTRL_H = 36.f;
    static constexpr float SPEED_Y = 230.f;
    static constexpr float TOOL_Y = 300.f;
    static constexpr float TOOL_W = 90.f;
    static constexpr float TOOL_H = 32.f;
    static constexpr float TOOL_GAP_X = 100.f;
    static constexpr float TOOL_GAP_Y = 42.f;
    static constexpr float METRIC_Y = 480.f;

public:
    bool loadFont() {
        if (font.loadFromFile("C:/Windows/Fonts/segoeui.ttf") ||
            font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;
            return true;
        }
        return false;
    }

    void updateHover(int x, int y) { mouseX = x; mouseY = y; }
    void setRunning(bool r) { isRunning = r; }
    void setPaused(bool p) { isPaused = p; }
    void setMetrics(int n, int p, int c, long long t) {
        m_nodes = n; m_pathLen = p; m_cost = c; m_time = t;
    }

    std::string getSelectedAlgo() const { return selectedAlgo; }
    std::string getSelectedTool() const { return selectedTool; }
    int getSpeed() const { return speedMs; }

    std::string handleClick(int x, int y) {
        if (x < startX) {
            dropdownOpen = false;
            return "";
        }

        // --- Dropdown header ---
        sf::FloatRect dropRect(startX + PAD, DROP_Y, UI_PANEL_WIDTH - PAD * 2, DROP_H);
        if (dropRect.contains((float)x, (float)y)) {
            dropdownOpen = !dropdownOpen;
            return "";
        }

        // --- Dropdown list (only when open) ---
        if (dropdownOpen) {
            for (size_t i = 0; i < algos.size(); i++) {
                sf::FloatRect itemRect(startX + PAD, DROP_Y + DROP_H + i * DROP_H,
                                       UI_PANEL_WIDTH - PAD * 2, DROP_H);
                if (itemRect.contains((float)x, (float)y)) {
                    selectedAlgo = algos[i];
                    dropdownOpen = false;
                    return "ALGO_CHANGE";
                }
            }
            // click elsewhere closes list; still allow other controls below
            dropdownOpen = false;
        }

        // --- Run / Pause / Reset ---
        float btnW = 90.f;
        if (sf::FloatRect(startX + PAD, CTRL_Y, btnW, CTRL_H).contains((float)x, (float)y))
            return "CMD_RUN";
        if (sf::FloatRect(startX + PAD + 100.f, CTRL_Y, btnW, CTRL_H).contains((float)x, (float)y))
            return "CMD_PAUSE";
        if (sf::FloatRect(startX + PAD + 200.f, CTRL_Y, btnW, CTRL_H).contains((float)x, (float)y))
            return "CMD_RESET";

        // --- Speed slider ---
        if (sf::FloatRect(startX + PAD, SPEED_Y, UI_PANEL_WIDTH - PAD * 2, 24.f).contains((float)x, (float)y)) {
            float percent = (x - (startX + PAD)) / (UI_PANEL_WIDTH - PAD * 2);
            if (percent < 0.f) percent = 0.f;
            if (percent > 1.f) percent = 1.f;
            speedMs = MAX_SPEED - (int)(percent * (MAX_SPEED - MIN_SPEED));
            if (speedMs < MIN_SPEED) speedMs = MIN_SPEED;
            if (speedMs > MAX_SPEED) speedMs = MAX_SPEED;
            return "SPEED_CHANGE";
        }

        // --- Tools (3 per row) ---
        for (size_t i = 0; i < tools.size(); i++) {
            int row = (int)i / 3;
            int col = (int)i % 3;
            float tx = startX + PAD + col * TOOL_GAP_X;
            float ty = TOOL_Y + row * TOOL_GAP_Y;
            if (sf::FloatRect(tx, ty, TOOL_W, TOOL_H).contains((float)x, (float)y)) {
                selectedTool = tools[i];
                return "TOOL_CHANGE";
            }
        }

        return "";
    }

    void render(sf::RenderWindow& window) {
        sf::RectangleShape panel(sf::Vector2f((float)UI_PANEL_WIDTH, (float)WINDOW_HEIGHT));
        panel.setPosition(startX, 0.f);
        panel.setFillColor(Colors::BG_PANEL);
        window.draw(panel);

        sf::RectangleShape border(sf::Vector2f(1.f, (float)WINDOW_HEIGHT));
        border.setPosition(startX, 0.f);
        border.setFillColor(Colors::BORDER_DIM);
        window.draw(border);

        if (!fontLoaded) return;

        drawText(window, "ALGORITHM", startX + PAD, 30.f, 12, Colors::TEXT_MUTED, true);

        // Dropdown header
        sf::FloatRect dropRect(startX + PAD, DROP_Y, UI_PANEL_WIDTH - PAD * 2, DROP_H);
        bool dropHover = dropRect.contains((float)mouseX, (float)mouseY);
        drawButtonRect(window, dropRect, dropHover || dropdownOpen,
                       dropdownOpen ? Colors::ACCENT_TEAL : sf::Color::Transparent);
        drawText(window, selectedAlgo, startX + PAD + 15.f, DROP_Y + 8.f, 14, Colors::TEXT_PRIMARY, true);
        drawText(window, dropdownOpen ? "^" : "v", startX + UI_PANEL_WIDTH - 40.f, DROP_Y + 8.f, 14, Colors::TEXT_MUTED);

        // Controls
        drawText(window, "SIMULATION CONTROLS", startX + PAD, CTRL_Y - 28.f, 12, Colors::TEXT_MUTED, true);
        drawControlButton(window, "Run", startX + PAD, CTRL_Y, Colors::ACCENT_GREEN, !isRunning || isPaused);
        drawControlButton(window, isPaused ? "Resume" : "Pause", startX + PAD + 100.f, CTRL_Y, Colors::SAND, isRunning);
        drawControlButton(window, "Reset", startX + PAD + 200.f, CTRL_Y, Colors::ACCENT_RED, true);

        // Speed
        drawText(window, "Execution Speed", startX + PAD, SPEED_Y - 22.f, 12, Colors::TEXT_MUTED);
        drawText(window, std::to_string(speedMs) + "ms", startX + UI_PANEL_WIDTH - 55.f, SPEED_Y - 22.f, 12, Colors::ACCENT_TEAL);

        sf::RectangleShape track(sf::Vector2f(UI_PANEL_WIDTH - PAD * 2, 4.f));
        track.setPosition(startX + PAD, SPEED_Y + 10.f);
        track.setFillColor(Colors::BORDER_DIM);
        window.draw(track);

        float percent = 1.0f - (float)(speedMs - MIN_SPEED) / (float)(MAX_SPEED - MIN_SPEED);
        if (percent < 0.f) percent = 0.f;
        if (percent > 1.f) percent = 1.f;
        sf::RectangleShape fill(sf::Vector2f((UI_PANEL_WIDTH - PAD * 2) * percent, 4.f));
        fill.setPosition(startX + PAD, SPEED_Y + 10.f);
        fill.setFillColor(Colors::ACCENT_TEAL);
        window.draw(fill);

        sf::CircleShape thumb(8.f);
        thumb.setPosition(startX + PAD + (UI_PANEL_WIDTH - PAD * 2) * percent - 8.f, SPEED_Y + 4.f);
        thumb.setFillColor(Colors::TEXT_PRIMARY);
        window.draw(thumb);

        // Tools
        drawText(window, "TERRAIN BRUSHES", startX + PAD, TOOL_Y - 28.f, 12, Colors::TEXT_MUTED, true);
        for (size_t i = 0; i < tools.size(); i++) {
            int row = (int)i / 3;
            int col = (int)i % 3;
            float tx = startX + PAD + col * TOOL_GAP_X;
            float ty = TOOL_Y + row * TOOL_GAP_Y;
            sf::FloatRect r(tx, ty, TOOL_W, TOOL_H);
            bool hover = r.contains((float)mouseX, (float)mouseY);
            bool active = (selectedTool == tools[i]);
            drawButtonRect(window, r, hover || active,
                           active ? Colors::ACCENT_PURPLE : sf::Color::Transparent);
            drawText(window, tools[i], tx + 14.f, ty + 6.f, 13,
                     active ? Colors::TEXT_PRIMARY : Colors::TEXT_SECONDARY);
        }

        // Metrics
        drawText(window, "REAL-TIME METRICS", startX + PAD, METRIC_Y - 28.f, 12, Colors::TEXT_MUTED, true);
        drawMetricBox(window, startX + PAD, METRIC_Y, "Nodes Explored", std::to_string(m_nodes));
        drawMetricBox(window, startX + PAD + 150.f, METRIC_Y, "Path Length", std::to_string(m_pathLen));
        drawMetricBox(window, startX + PAD, METRIC_Y + 70.f, "Path Cost", std::to_string(m_cost));
        drawMetricBox(window, startX + PAD + 150.f, METRIC_Y + 70.f, "Exec Time", std::to_string(m_time) + "ms");

        // Dropdown list ON TOP
        if (dropdownOpen) {
            for (size_t i = 0; i < algos.size(); i++) {
                sf::FloatRect r(startX + PAD, DROP_Y + DROP_H + i * DROP_H,
                                UI_PANEL_WIDTH - PAD * 2, DROP_H);
                bool hover = r.contains((float)mouseX, (float)mouseY);
                sf::RectangleShape bg(sf::Vector2f(r.width, r.height));
                bg.setPosition(r.left, r.top);
                bg.setFillColor(hover ? Colors::UI_HOVER : Colors::BG_APP);
                bg.setOutlineThickness(1.f);
                bg.setOutlineColor(Colors::BORDER_DIM);
                window.draw(bg);
                drawText(window, algos[i], r.left + 15.f, r.top + 8.f, 14, Colors::TEXT_PRIMARY);
            }
        }
    }

private:
    void drawText(sf::RenderWindow& w, const std::string& str, float x, float y,
                  unsigned size, sf::Color color, bool bold = false) {
        sf::Text text;
        text.setFont(font);
        text.setString(str);
        text.setCharacterSize(size);
        text.setFillColor(color);
        text.setPosition(x, y);
        if (bold) text.setStyle(sf::Text::Bold);
        w.draw(text);
    }

    void drawButtonRect(sf::RenderWindow& w, sf::FloatRect r, bool hover, sf::Color outline) {
        sf::RectangleShape rect(sf::Vector2f(r.width, r.height));
        rect.setPosition(r.left, r.top);
        rect.setFillColor(hover ? Colors::UI_HOVER : Colors::UI_ELEMENT);
        rect.setOutlineThickness(1.f);
        rect.setOutlineColor(outline == sf::Color::Transparent ? Colors::BORDER_DIM : outline);
        w.draw(rect);
    }

    void drawControlButton(sf::RenderWindow& w, const std::string& label, float x, float y,
                           sf::Color accent, bool active) {
        sf::FloatRect r(x, y, 90.f, CTRL_H);
        bool hover = r.contains((float)mouseX, (float)mouseY) && active;
        sf::RectangleShape rect(sf::Vector2f(r.width, r.height));
        rect.setPosition(r.left, r.top);
        rect.setFillColor(hover ? Colors::UI_HOVER : Colors::UI_ELEMENT);
        rect.setOutlineThickness(1.f);
        rect.setOutlineColor(active ? accent : Colors::BORDER_DIM);
        w.draw(rect);
        drawText(w, label, x + 18.f, y + 8.f, 14, active ? Colors::TEXT_PRIMARY : Colors::TEXT_MUTED, true);
    }

    void drawMetricBox(sf::RenderWindow& w, float x, float y, const std::string& label, const std::string& value) {
        sf::RectangleShape box(sf::Vector2f(140.f, 60.f));
        box.setPosition(x, y);
        box.setFillColor(Colors::BG_APP);
        box.setOutlineThickness(1.f);
        box.setOutlineColor(Colors::BORDER_DIM);
        w.draw(box);
        drawText(w, label, x + 10.f, y + 8.f, 11, Colors::TEXT_MUTED);
        drawText(w, value, x + 10.f, y + 28.f, 20, Colors::ACCENT_TEAL, true);
    }
};