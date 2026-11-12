#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "../rendering/Colors.h"
#include "../core/Constants.h"

enum class PanelMode { STANDARD, RACE };

class InfoPanel {
private:
    sf::Font font;
    bool fontLoaded = false;
    int mouseX = 0, mouseY = 0;
    float startX = (float)MAZE_WIDTH;

    PanelMode currentMode = PanelMode::STANDARD;

    // Standard State
    std::string selectedAlgo = "A*";
    bool dropdownOpen = false;

    // Maze Generator State
    std::vector<std::string> m_genAlgos = { "Backtracker", "Prim's", "Division" };
    int m_genAlgoIdx = 0;

    sf::RectangleShape m_genAlgoBtn;
    sf::Text m_genAlgoText;
    bool m_genAlgoHovered = false;

    sf::RectangleShape m_genRunBtn;
    sf::Text m_genRunText;
    bool m_genRunHovered = false;

    // Race State
    std::string selectedAlgoA = "A*";
    std::string selectedAlgoB = "BFS";
    bool dropdownAOpen = false;
    bool dropdownBOpen = false;

    // Shared State
    std::string selectedTool = "Wall";
    bool isRunning = false;
    bool isPaused = false;
    int speedMs = 30;

    std::vector<std::string> algos = {"BFS", "DFS", "A*", "Dijkstra", "Greedy", "BiDir BFS", "IDDFS"};
    std::vector<std::string> tools = {"Wall", "Erase", "Start", "End", "Sand", "Water", "Mud"};

    // Metrics
    int m_nodes = 0, m_pathLen = 0, m_cost = 0; long long m_time = 0;
    int m_nodesB = 0, m_pathLenB = 0, m_costB = 0; long long m_timeB = 0;

    // Layout
    static constexpr float PAD = 20.f;
    static constexpr float CTRL_H = 36.f;
    static constexpr float DROP_H = 36.f;

    void layoutButtons() {
        if (!fontLoaded) return;

        float panelX = startX;
        float pad = PAD;
        float btnW = (float)UI_PANEL_WIDTH - pad * 2.f;
        float btnH = 32.f;

        // Maze Generator layout positioned below metrics/brushes
        float y = 535.f;

        m_genAlgoBtn.setSize(sf::Vector2f(btnW, btnH));
        m_genAlgoBtn.setPosition(panelX + pad, y);
        m_genAlgoBtn.setFillColor(Colors::UI_ELEMENT);
        m_genAlgoBtn.setOutlineThickness(1.f);
        m_genAlgoBtn.setOutlineColor(Colors::BORDER_DIM);

        m_genAlgoText.setFont(font);
        m_genAlgoText.setString("Algo: " + m_genAlgos[m_genAlgoIdx]);
        m_genAlgoText.setCharacterSize(14);
        m_genAlgoText.setFillColor(Colors::TEXT_PRIMARY);
        centerText(m_genAlgoText, m_genAlgoBtn);

        y += btnH + 8.f;

        m_genRunBtn.setSize(sf::Vector2f(btnW, btnH));
        m_genRunBtn.setPosition(panelX + pad, y);
        m_genRunBtn.setFillColor(sf::Color(21, 128, 61));
        m_genRunBtn.setOutlineThickness(1.f);
        m_genRunBtn.setOutlineColor(Colors::BORDER_DIM);

        m_genRunText.setFont(font);
        m_genRunText.setString("Generate Maze");
        m_genRunText.setCharacterSize(14);
        m_genRunText.setFillColor(Colors::TEXT_PRIMARY);
        centerText(m_genRunText, m_genRunBtn);
    }

    void centerText(sf::Text& text, const sf::RectangleShape& btn) {
        sf::FloatRect tb = text.getLocalBounds();
        sf::FloatRect bb = btn.getGlobalBounds();
        text.setPosition(
            bb.left + (bb.width - tb.width) * 0.5f - tb.left,
            bb.top + (bb.height - tb.height) * 0.5f - tb.top - 1.f
        );
    }

public:
    bool loadFont() {
        if (font.loadFromFile("C:/Windows/Fonts/segoeui.ttf") || font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;
            layoutButtons();
            return true;
        }
        return false;
    }

    void updateHover(int x, int y) {
        mouseX = x;
        mouseY = y;

        if (currentMode == PanelMode::STANDARD) {
            m_genAlgoHovered = m_genAlgoBtn.getGlobalBounds().contains(static_cast<float>(mouseX), static_cast<float>(mouseY));
            m_genAlgoBtn.setFillColor(m_genAlgoHovered ? Colors::UI_HOVER : Colors::UI_ELEMENT);

            m_genRunHovered = m_genRunBtn.getGlobalBounds().contains(static_cast<float>(mouseX), static_cast<float>(mouseY));
            m_genRunBtn.setFillColor(m_genRunHovered ? sf::Color(34, 197, 94) : sf::Color(21, 128, 61));
        }
    }

    void setRunning(bool r) { isRunning = r; }
    void setPaused(bool p) { isPaused = p; }
    
    void setMetrics(int n, int p, int c, long long t) { m_nodes = n; m_pathLen = p; m_cost = c; m_time = t; }
    void setMetricsB(int n, int p, int c, long long t) { m_nodesB = n; m_pathLenB = p; m_costB = c; m_timeB = t; }

    std::string getSelectedAlgo() const { return selectedAlgo; }
    std::string getSelectedAlgoA() const { return selectedAlgoA; }
    std::string getSelectedAlgoB() const { return selectedAlgoB; }
    std::string getSelectedTool() const { return selectedTool; }
    std::string getSelectedGenAlgo() const { return m_genAlgos[m_genAlgoIdx]; }
    int getSpeed() const { return speedMs; }
    PanelMode getMode() const { return currentMode; }

    std::string handleClick(int x, int y) {
        if (x < startX) { dropdownOpen = dropdownAOpen = dropdownBOpen = false; return ""; }

        if (currentMode == PanelMode::STANDARD) {
            // Check generator clicks first
            if (m_genAlgoHovered) {
                m_genAlgoIdx = (m_genAlgoIdx + 1) % m_genAlgos.size();
                m_genAlgoText.setString("Algo: " + m_genAlgos[m_genAlgoIdx]);
                centerText(m_genAlgoText, m_genAlgoBtn);
                return "CMD_GEN_ALGO_CHANGE";
            }
            if (m_genRunHovered) {
                return "CMD_GEN_MAZE";
            }

            // Dropdown Standard
            sf::FloatRect dropRect(startX + PAD, 60.f, UI_PANEL_WIDTH - PAD * 2, DROP_H);
            if (dropRect.contains((float)x, (float)y)) { dropdownOpen = !dropdownOpen; return ""; }

            if (dropdownOpen) {
                for (size_t i = 0; i < algos.size(); i++) {
                    if (sf::FloatRect(startX + PAD, 60.f + DROP_H + i * DROP_H, UI_PANEL_WIDTH - PAD * 2, DROP_H).contains((float)x, (float)y)) {
                        selectedAlgo = algos[i]; dropdownOpen = false; return "ALGO_CHANGE";
                    }
                }
                dropdownOpen = false;
            }

            // Controls Standard
            if (sf::FloatRect(startX + PAD, 160.f, 90.f, CTRL_H).contains((float)x, (float)y)) return "CMD_RUN";
            if (sf::FloatRect(startX + PAD + 100.f, 160.f, 90.f, CTRL_H).contains((float)x, (float)y)) return "CMD_PAUSE";
            if (sf::FloatRect(startX + PAD + 200.f, 160.f, 90.f, CTRL_H).contains((float)x, (float)y)) return "CMD_RESET";

            // Bottom Buttons: Race Mode & Race All
            if (sf::FloatRect(startX + PAD, 645.f, 140.f, CTRL_H).contains((float)x, (float)y)) {
                currentMode = PanelMode::RACE; return "CMD_RESET"; 
            }
            if (sf::FloatRect(startX + PAD + 150.f, 645.f, 140.f, CTRL_H).contains((float)x, (float)y)) {
                return "CMD_RACE_ALL";
            }

        } else {
            // RACE MODE
            sf::FloatRect dropA(startX + PAD, 60.f, UI_PANEL_WIDTH - PAD * 2, DROP_H);
            sf::FloatRect dropB(startX + PAD, 130.f, UI_PANEL_WIDTH - PAD * 2, DROP_H);

            if (dropA.contains((float)x, (float)y)) { dropdownAOpen = !dropdownAOpen; dropdownBOpen = false; return ""; }
            if (dropB.contains((float)x, (float)y)) { dropdownBOpen = !dropdownBOpen; dropdownAOpen = false; return ""; }

            if (dropdownAOpen) {
                for (size_t i = 0; i < algos.size(); i++) {
                    if (sf::FloatRect(startX + PAD, 60.f + DROP_H + i * DROP_H, UI_PANEL_WIDTH - PAD * 2, DROP_H).contains((float)x, (float)y)) {
                        selectedAlgoA = algos[i]; dropdownAOpen = false; return "ALGO_CHANGE";
                    }
                }
                dropdownAOpen = false;
            }
            if (dropdownBOpen) {
                for (size_t i = 0; i < algos.size(); i++) {
                    if (sf::FloatRect(startX + PAD, 130.f + DROP_H + i * DROP_H, UI_PANEL_WIDTH - PAD * 2, DROP_H).contains((float)x, (float)y)) {
                        selectedAlgoB = algos[i]; dropdownBOpen = false; return "ALGO_CHANGE";
                    }
                }
                dropdownBOpen = false;
            }

            // Controls Race
            if (sf::FloatRect(startX + PAD, 190.f, 90.f, CTRL_H).contains((float)x, (float)y)) return "CMD_RUN";
            if (sf::FloatRect(startX + PAD + 100.f, 190.f, 90.f, CTRL_H).contains((float)x, (float)y)) return "CMD_PAUSE";
            if (sf::FloatRect(startX + PAD + 200.f, 190.f, 90.f, CTRL_H).contains((float)x, (float)y)) return "CMD_RESET";

            // Exit Race Mode
            if (sf::FloatRect(startX + PAD, 645.f, UI_PANEL_WIDTH - PAD * 2, CTRL_H).contains((float)x, (float)y)) {
                currentMode = PanelMode::STANDARD; return "CMD_RESET";
            }
        }

        // Shared Speed Slider
        float speedY = (currentMode == PanelMode::STANDARD) ? 230.f : 260.f;
        if (sf::FloatRect(startX + PAD, speedY, UI_PANEL_WIDTH - PAD * 2, 24.f).contains((float)x, (float)y)) {
            float percent = (x - (startX + PAD)) / (UI_PANEL_WIDTH - PAD * 2);
            if (percent < 0.f) percent = 0.f; if (percent > 1.f) percent = 1.f;
            speedMs = MAX_SPEED - (int)(percent * (MAX_SPEED - MIN_SPEED));
            if (speedMs < MIN_SPEED) speedMs = MIN_SPEED; if (speedMs > MAX_SPEED) speedMs = MAX_SPEED;
            return "SPEED_CHANGE";
        }

        // Tools
        if (currentMode == PanelMode::STANDARD) {
            for (size_t i = 0; i < tools.size(); i++) {
                float tx = startX + PAD + ((int)i % 3) * 100.f;
                float ty = 300.f + ((int)i / 3) * 42.f;
                if (sf::FloatRect(tx, ty, 90.f, 32.f).contains((float)x, (float)y)) {
                    selectedTool = tools[i]; return "TOOL_CHANGE";
                }
            }
        }
        return "";
    }

    void render(sf::RenderWindow& w) {
        sf::RectangleShape panel(sf::Vector2f((float)UI_PANEL_WIDTH, (float)WINDOW_HEIGHT));
        panel.setPosition(startX, 0.f);
        panel.setFillColor(Colors::BG_PANEL);
        w.draw(panel);
        sf::RectangleShape border(sf::Vector2f(1.f, (float)WINDOW_HEIGHT));
        border.setPosition(startX, 0.f);
        border.setFillColor(Colors::BORDER_DIM);
        w.draw(border);

        if (!fontLoaded) return;

        if (currentMode == PanelMode::STANDARD) {
            renderStandard(w);
        } else {
            renderRace(w);
        }
    }

private:
    void renderStandard(sf::RenderWindow& w) {
        drawText(w, "ALGORITHM", startX + PAD, 30.f, 12, Colors::TEXT_MUTED, true);
        
        sf::FloatRect dropRect(startX + PAD, 60.f, UI_PANEL_WIDTH - PAD * 2, DROP_H);
        drawButtonRect(w, dropRect, dropRect.contains((float)mouseX, (float)mouseY) || dropdownOpen, dropdownOpen ? Colors::ACCENT_TEAL : sf::Color::Transparent);
        drawText(w, selectedAlgo, startX + PAD + 15.f, 68.f, 14, Colors::TEXT_PRIMARY, true);
        drawText(w, dropdownOpen ? "^" : "v", startX + UI_PANEL_WIDTH - 40.f, 68.f, 14, Colors::TEXT_MUTED);

        drawText(w, "SIMULATION CONTROLS", startX + PAD, 132.f, 12, Colors::TEXT_MUTED, true);
        drawControlButton(w, "Run", startX + PAD, 160.f, Colors::ACCENT_GREEN, !isRunning || isPaused);
        drawControlButton(w, isPaused ? "Resume" : "Pause", startX + PAD + 100.f, 160.f, Colors::SAND, isRunning);
        drawControlButton(w, "Reset", startX + PAD + 200.f, 160.f, Colors::ACCENT_RED, true);

        renderSpeed(w, 230.f);

        drawText(w, "TERRAIN BRUSHES", startX + PAD, 272.f, 12, Colors::TEXT_MUTED, true);
        for (size_t i = 0; i < tools.size(); i++) {
            float tx = startX + PAD + ((int)i % 3) * 100.f;
            float ty = 300.f + ((int)i / 3) * 42.f;
            sf::FloatRect r(tx, ty, 90.f, 32.f);
            bool hover = r.contains((float)mouseX, (float)mouseY);
            bool active = (selectedTool == tools[i]);
            drawButtonRect(w, r, hover || active, active ? Colors::ACCENT_PURPLE : sf::Color::Transparent);
            drawText(w, tools[i], tx + 14.f, ty + 6.f, 13, active ? Colors::TEXT_PRIMARY : Colors::TEXT_SECONDARY);
        }

        drawText(w, "REAL-TIME METRICS", startX + PAD, 432.f, 12, Colors::TEXT_MUTED, true);
        drawMetricBox(w, startX + PAD, 455.f, "Nodes Explored", std::to_string(m_nodes));
        drawMetricBox(w, startX + PAD + 150.f, 455.f, "Path Length", std::to_string(m_pathLen));

        // Maze Generator Section
        drawText(w, "MAZE GENERATOR", startX + PAD, 520.f, 12, Colors::TEXT_MUTED, true);
        w.draw(m_genAlgoBtn);
        w.draw(m_genAlgoText);
        w.draw(m_genRunBtn);
        w.draw(m_genRunText);

        drawControlButton(w, "Race Mode", startX + PAD, 645.f, Colors::ACCENT_PURPLE, true);
        drawControlButton(w, "Race All", startX + PAD + 150.f, 645.f, Colors::OVERLAP_PATH, true);

        if (dropdownOpen) renderDropdownList(w, 60.f, selectedAlgo, algos, false);
    }

    void renderRace(sf::RenderWindow& w) {
        drawText(w, "RACE MODE", startX + PAD, 30.f, 14, Colors::OVERLAP_PATH, true);
        
        drawText(w, "Algorithm A", startX + PAD, 60.f - 16.f, 11, Colors::TEXT_MUTED);
        sf::FloatRect dropA(startX + PAD, 60.f, UI_PANEL_WIDTH - PAD * 2, DROP_H);
        drawButtonRect(w, dropA, dropA.contains((float)mouseX, (float)mouseY) || dropdownAOpen, dropdownAOpen ? Colors::ACCENT_TEAL : sf::Color::Transparent);
        drawText(w, selectedAlgoA, startX + PAD + 15.f, 68.f, 14, Colors::TEXT_PRIMARY, true);

        drawText(w, "Algorithm B", startX + PAD, 130.f - 16.f, 11, Colors::TEXT_MUTED);
        sf::FloatRect dropB(startX + PAD, 130.f, UI_PANEL_WIDTH - PAD * 2, DROP_H);
        drawButtonRect(w, dropB, dropB.contains((float)mouseX, (float)mouseY) || dropdownBOpen, dropdownBOpen ? Colors::ACCENT_TEAL : sf::Color::Transparent);
        drawText(w, selectedAlgoB, startX + PAD + 15.f, 138.f, 14, Colors::TEXT_PRIMARY, true);

        drawControlButton(w, "Start Race", startX + PAD, 190.f, Colors::ACCENT_GREEN, !isRunning || isPaused);
        drawControlButton(w, isPaused ? "Resume" : "Pause", startX + PAD + 100.f, 190.f, Colors::SAND, isRunning);
        drawControlButton(w, "Reset", startX + PAD + 200.f, 190.f, Colors::ACCENT_RED, true);

        renderSpeed(w, 260.f);

        drawText(w, "METRICS A (" + selectedAlgoA + ")", startX + PAD, 310.f, 12, Colors::TEXT_MUTED, true);
        drawMetricBox(w, startX + PAD, 330.f, "Nodes Explored", std::to_string(m_nodes));
        drawMetricBox(w, startX + PAD + 150.f, 330.f, "Path Cost", std::to_string(m_cost));
        drawMetricBox(w, startX + PAD, 395.f, "Path Length", std::to_string(m_pathLen));
        drawMetricBox(w, startX + PAD + 150.f, 395.f, "Exec Time", std::to_string(m_time) + "ms");

        drawText(w, "METRICS B (" + selectedAlgoB + ")", startX + PAD, 470.f, 12, Colors::TEXT_MUTED, true);
        drawMetricBox(w, startX + PAD, 490.f, "Nodes Explored", std::to_string(m_nodesB));
        drawMetricBox(w, startX + PAD + 150.f, 490.f, "Path Cost", std::to_string(m_costB));
        drawMetricBox(w, startX + PAD, 555.f, "Path Length", std::to_string(m_pathLenB));
        drawMetricBox(w, startX + PAD + 150.f, 555.f, "Exec Time", std::to_string(m_timeB) + "ms");

        // Exit button
        sf::FloatRect exitBtn(startX + PAD, 645.f, UI_PANEL_WIDTH - PAD * 2, CTRL_H);
        drawButtonRect(w, exitBtn, exitBtn.contains((float)mouseX, (float)mouseY), Colors::BORDER_DIM);
        drawText(w, "< Exit Race Mode", startX + PAD + 80.f, 653.f, 14, Colors::TEXT_MUTED, true);

        if (dropdownBOpen) renderDropdownList(w, 130.f, selectedAlgoB, algos, false);
        if (dropdownAOpen) renderDropdownList(w, 60.f, selectedAlgoA, algos, false);
    }

    void renderSpeed(sf::RenderWindow& w, float y) {
        drawText(w, "Execution Speed", startX + PAD, y - 22.f, 12, Colors::TEXT_MUTED);
        drawText(w, std::to_string(speedMs) + "ms", startX + UI_PANEL_WIDTH - 55.f, y - 22.f, 12, Colors::ACCENT_TEAL);
        sf::RectangleShape track(sf::Vector2f(UI_PANEL_WIDTH - PAD * 2, 4.f));
        track.setPosition(startX + PAD, y + 10.f); track.setFillColor(Colors::BORDER_DIM); w.draw(track);
        float percent = 1.0f - (float)(speedMs - MIN_SPEED) / (float)(MAX_SPEED - MIN_SPEED);
        sf::RectangleShape fill(sf::Vector2f((UI_PANEL_WIDTH - PAD * 2) * percent, 4.f));
        fill.setPosition(startX + PAD, y + 10.f); fill.setFillColor(Colors::ACCENT_TEAL); w.draw(fill);
        sf::CircleShape thumb(8.f);
        thumb.setPosition(startX + PAD + (UI_PANEL_WIDTH - PAD * 2) * percent - 8.f, y + 4.f);
        thumb.setFillColor(Colors::TEXT_PRIMARY); w.draw(thumb);
    }

    void renderDropdownList(sf::RenderWindow& w, float startY, std::string& current, std::vector<std::string>& list, bool isUp) {
        for (size_t i = 0; i < list.size(); i++) {
            sf::FloatRect r(startX + PAD, startY + DROP_H + i * DROP_H, UI_PANEL_WIDTH - PAD * 2, DROP_H);
            bool hover = r.contains((float)mouseX, (float)mouseY);
            sf::RectangleShape bg(sf::Vector2f(r.width, r.height));
            bg.setPosition(r.left, r.top);
            bg.setFillColor(hover ? Colors::UI_HOVER : Colors::BG_APP);
            bg.setOutlineThickness(1.f); bg.setOutlineColor(Colors::BORDER_DIM);
            w.draw(bg);
            drawText(w, list[i], r.left + 15.f, r.top + 8.f, 14, Colors::TEXT_PRIMARY);
        }
    }

    void drawText(sf::RenderWindow& w, const std::string& s, float x, float y, unsigned size, sf::Color color, bool bold = false) {
        sf::Text t; t.setFont(font); t.setString(s); t.setCharacterSize(size); t.setFillColor(color); t.setPosition(x, y);
        if (bold) t.setStyle(sf::Text::Bold); w.draw(t);
    }
    void drawButtonRect(sf::RenderWindow& w, sf::FloatRect r, bool hover, sf::Color outline) {
        sf::RectangleShape rect(sf::Vector2f(r.width, r.height)); rect.setPosition(r.left, r.top);
        rect.setFillColor(hover ? Colors::UI_HOVER : Colors::UI_ELEMENT); rect.setOutlineThickness(1.f);
        rect.setOutlineColor(outline == sf::Color::Transparent ? Colors::BORDER_DIM : outline); w.draw(rect);
    }
    void drawControlButton(sf::RenderWindow& w, const std::string& label, float x, float y, sf::Color accent, bool active) {
        sf::FloatRect r(x, y, (label=="Race Mode"||label=="Race All")?140.f:90.f, CTRL_H);
        bool hover = r.contains((float)mouseX, (float)mouseY) && active;
        sf::RectangleShape rect(sf::Vector2f(r.width, r.height)); rect.setPosition(r.left, r.top);
        rect.setFillColor(hover ? Colors::UI_HOVER : Colors::UI_ELEMENT); rect.setOutlineThickness(1.f);
        rect.setOutlineColor(active ? accent : Colors::BORDER_DIM); w.draw(rect);
        drawText(w, label, x + (r.width/2.f) - (label.length()*3.5f), y + 8.f, 14, active ? Colors::TEXT_PRIMARY : Colors::TEXT_MUTED, true);
    }
    void drawMetricBox(sf::RenderWindow& w, float x, float y, const std::string& label, const std::string& value) {
        sf::RectangleShape box(sf::Vector2f(140.f, 50.f)); box.setPosition(x, y); box.setFillColor(Colors::BG_APP);
        box.setOutlineThickness(1.f); box.setOutlineColor(Colors::BORDER_DIM); w.draw(box);
        drawText(w, label, x + 10.f, y + 6.f, 11, Colors::TEXT_MUTED);
        drawText(w, value, x + 10.f, y + 20.f, 18, Colors::ACCENT_TEAL, true);
    }
};