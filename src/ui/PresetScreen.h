#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "../core/Constants.h"
#include "../rendering/Colors.h"
#include "../mazes/PresetMazes.h"

// Flow: DIFFICULTY -> LEVEL -> done (caller loads maze)
enum class PresetStage { DIFFICULTY, LEVEL };

class PresetScreen {
private:
    sf::Font font;
    bool fontLoaded = false;
    PresetStage stage = PresetStage::DIFFICULTY;
    Difficulty selectedDiff = Difficulty::BEGINNER;
    int hoverIndex = -1; // 0..2 difficulty, or 0..4 level
    int mouseX = 0, mouseY = 0;

public:
    bool loadFont() {
        if (font.loadFromFile("C:/Windows/Fonts/segoeui.ttf") ||
            font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;
            return true;
        }
        return false;
    }

    void reset() {
        stage = PresetStage::DIFFICULTY;
        hoverIndex = -1;
    }

    void updateHover(int mx, int my) {
        mouseX = mx;
        mouseY = my;
        hoverIndex = -1;

        if (stage == PresetStage::DIFFICULTY) {
            for (int i = 0; i < 3; i++) {
                if (diffRect(i).contains((float)mx, (float)my)) hoverIndex = i;
            }
        } else {
            for (int i = 0; i < 5; i++) {
                if (levelRect(i).contains((float)mx, (float)my)) hoverIndex = i;
            }
            if (backRect().contains((float)mx, (float)my)) hoverIndex = 100;
        }
    }

    // Returns:
    //  0 = nothing
    //  1 = back to startup (only if you wire it)
    //  2 = difficulty chosen (stay on screen, stage changed)
    //  10+level = level chosen (11..15) -> caller should load maze
    //  -1 = back from level to difficulty
    int handleClick(int mx, int my) {
        updateHover(mx, my);

        if (stage == PresetStage::DIFFICULTY) {
            if (hoverIndex >= 0 && hoverIndex <= 2) {
                if (hoverIndex == 0) selectedDiff = Difficulty::BEGINNER;
                if (hoverIndex == 1) selectedDiff = Difficulty::INTERMEDIATE;
                if (hoverIndex == 2) selectedDiff = Difficulty::ADVANCED;
                stage = PresetStage::LEVEL;
                return 2;
            }
            return 0;
        }

        // LEVEL stage
        if (hoverIndex == 100) {
            stage = PresetStage::DIFFICULTY;
            return -1;
        }
        if (hoverIndex >= 0 && hoverIndex <= 4) {
            return 10 + (hoverIndex + 1); // 11..15 for levels 1..5
        }
        return 0;
    }

    Difficulty getDifficulty() const { return selectedDiff; }

    void render(sf::RenderWindow& window) {
        sf::RectangleShape bg(sf::Vector2f((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT));
        bg.setFillColor(Colors::BG_APP);
        window.draw(bg);
        if (!fontLoaded) return;

        if (stage == PresetStage::DIFFICULTY) drawDifficulty(window);
        else drawLevels(window);
    }

private:
    sf::FloatRect diffRect(int i) const {
        float w = 280.f, h = 120.f;
        float gap = 40.f;
        float total = 3 * w + 2 * gap;
        float x0 = (WINDOW_WIDTH - total) / 2.f;
        float y = 320.f;
        return sf::FloatRect(x0 + i * (w + gap), y, w, h);
    }

    sf::FloatRect levelRect(int i) const {
        float w = 520.f, h = 56.f;
        float x = (WINDOW_WIDTH - w) / 2.f;
        float y = 220.f + i * (h + 14.f);
        return sf::FloatRect(x, y, w, h);
    }

    sf::FloatRect backRect() const {
        return sf::FloatRect(40.f, 40.f, 120.f, 40.f);
    }

    void drawText(sf::RenderWindow& window, const std::string& s, float x, float y,
                  unsigned size, sf::Color color, bool bold = false) {
        sf::Text t;
        t.setFont(font);
        t.setString(s);
        t.setCharacterSize(size);
        t.setFillColor(color);
        t.setPosition(x, y);
        if (bold) t.setStyle(sf::Text::Bold);
        window.draw(t);
    }

    void drawDifficulty(sf::RenderWindow& window) {
        drawText(window, "PRESET SCENARIOS", WINDOW_WIDTH / 2.f - 140.f, 120.f, 36, Colors::TEXT_PRIMARY, true);
        drawText(window, "Select a difficulty", WINDOW_WIDTH / 2.f - 90.f, 180.f, 18, Colors::TEXT_SECONDARY);

        const char* labels[3] = {"Easy", "Medium", "Hard"};
        const char* sub[3] = {"Beginner  ·  5 mazes", "Intermediate  ·  5 mazes", "Advanced  ·  5 mazes"};
        sf::Color accents[3] = {Colors::ACCENT_GREEN, Colors::SAND, Colors::ACCENT_RED};

        for (int i = 0; i < 3; i++) {
            auto r = diffRect(i);
            bool hov = (hoverIndex == i);
            sf::RectangleShape card(sf::Vector2f(r.width, r.height));
            card.setPosition(r.left, r.top);
            card.setFillColor(hov ? Colors::UI_HOVER : Colors::BG_PANEL);
            card.setOutlineThickness(2.f);
            card.setOutlineColor(hov ? accents[i] : Colors::BORDER_DIM);
            window.draw(card);

            sf::RectangleShape bar(sf::Vector2f(r.width, 6.f));
            bar.setPosition(r.left, r.top);
            bar.setFillColor(accents[i]);
            window.draw(bar);

            drawText(window, labels[i], r.left + 24.f, r.top + 36.f, 28, Colors::TEXT_PRIMARY, true);
            drawText(window, sub[i], r.left + 24.f, r.top + 78.f, 14, Colors::TEXT_SECONDARY);
        }
    }

    void drawLevels(sf::RenderWindow& window) {
        // Back
        auto br = backRect();
        bool backHov = (hoverIndex == 100);
        sf::RectangleShape backBtn(sf::Vector2f(br.width, br.height));
        backBtn.setPosition(br.left, br.top);
        backBtn.setFillColor(backHov ? Colors::UI_HOVER : Colors::UI_ELEMENT);
        backBtn.setOutlineThickness(1.f);
        backBtn.setOutlineColor(Colors::BORDER_DIM);
        window.draw(backBtn);
        drawText(window, "< Back", br.left + 28.f, br.top + 10.f, 16, Colors::TEXT_PRIMARY, true);

        std::string title = "Easy";
        if (selectedDiff == Difficulty::INTERMEDIATE) title = "Medium";
        if (selectedDiff == Difficulty::ADVANCED) title = "Hard";
        drawText(window, title + " Mazes", WINDOW_WIDTH / 2.f - 80.f, 100.f, 32, Colors::TEXT_PRIMARY, true);
        drawText(window, "Select a level", WINDOW_WIDTH / 2.f - 60.f, 150.f, 16, Colors::TEXT_SECONDARY);

        for (int i = 0; i < 5; i++) {
            auto r = levelRect(i);
            bool hov = (hoverIndex == i);
            auto info = PresetMazes::getInfo(selectedDiff, i + 1);

            sf::RectangleShape row(sf::Vector2f(r.width, r.height));
            row.setPosition(r.left, r.top);
            row.setFillColor(hov ? Colors::UI_HOVER : Colors::BG_PANEL);
            row.setOutlineThickness(1.f);
            row.setOutlineColor(hov ? Colors::ACCENT_TEAL : Colors::BORDER_DIM);
            window.draw(row);

            drawText(window, "Lv " + std::to_string(i + 1), r.left + 20.f, r.top + 16.f, 16, Colors::ACCENT_TEAL, true);
            drawText(window, info.name, r.left + 90.f, r.top + 10.f, 18, Colors::TEXT_PRIMARY, true);
            drawText(window, info.description, r.left + 90.f, r.top + 32.f, 13, Colors::TEXT_MUTED);
        }
    }
};