#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include "../core/Constants.h"
#include "../rendering/Colors.h"

class StartupScreen {
private:
    sf::Font font;
    bool fontLoaded = false;
    float animTime = 0.0f;
    int hoverCard = 0; // 0=none, 1=Custom, 2=Preset

    // Card dimensions
    int cardW = 380, cardH = 240, cardY = 380, gap = 50;
    int startX = (WINDOW_WIDTH - (2 * 380 + 50)) / 2;

public:
    bool loadFont() {
        if (font.loadFromFile("C:/Windows/Fonts/segoeui.ttf") ||
            font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true; return true;
        }
        return false;
    }

    void update(float deltaTime, int mouseX, int mouseY) {
        animTime += deltaTime;
        
        // Hover detection
        hoverCard = 0;
        if (mouseY >= cardY && mouseY <= cardY + cardH) {
            if (mouseX >= startX && mouseX <= startX + cardW) hoverCard = 1;
            if (mouseX >= startX + cardW + gap && mouseX <= startX + cardW * 2 + gap) hoverCard = 2;
        }
    }

    int checkClick() {
        return hoverCard;
    }

    void render(sf::RenderWindow& window) {
        sf::RectangleShape bg(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        bg.setFillColor(Colors::BG_APP);
        window.draw(bg);

        if (!fontLoaded) return;

        // Animated Title
        float pulseAlpha = 220 + std::sin(animTime * 2) * 35;
        sf::Text title;
        title.setFont(font);
        title.setString("PATHFORGE");
        title.setCharacterSize(84);
        title.setFillColor(sf::Color(241, 245, 249, (sf::Uint8)pulseAlpha));
        title.setStyle(sf::Text::Bold);
        title.setLetterSpacing(1.3f);
        title.setPosition((WINDOW_WIDTH - title.getLocalBounds().width) / 2, 110);
        window.draw(title);

        sf::RectangleShape underline(sf::Vector2f(300 + std::sin(animTime * 1.5f) * 40, 3));
        underline.setPosition((WINDOW_WIDTH - underline.getSize().x) / 2, 210);
        underline.setFillColor(Colors::ACCENT_TEAL);
        window.draw(underline);

        sf::Text tagline;
        tagline.setFont(font);
        tagline.setString("Explore. Visualize. Understand.");
        tagline.setCharacterSize(20);
        tagline.setFillColor(Colors::TEXT_SECONDARY);
        tagline.setPosition((WINDOW_WIDTH - tagline.getLocalBounds().width) / 2, 240);
        window.draw(tagline);

        // Cards
        drawCard(window, startX, cardY, cardW, cardH, Colors::ACCENT_TEAL, "Custom Laboratory", 
                 "Design custom configurations, draw walls and terrain, and run algorithmic benchmarks.", hoverCard == 1);

        drawCard(window, startX + cardW + gap, cardY, cardW, cardH, Colors::ACCENT_PURPLE, "Preset Scenarios", 
                 "Load pre-configured test suites and curated maze types to test edge cases instantly.", hoverCard == 2);
    }

private:
    void drawCard(sf::RenderWindow& window, int x, int y, int w, int h, sf::Color accent, const std::string& title, const std::string& desc, bool isHovered) {
        sf::RectangleShape card(sf::Vector2f(w, h));
        card.setPosition(x, isHovered ? y - 10 : y);
        card.setFillColor(Colors::BG_PANEL);
        card.setOutlineThickness(2);
        card.setOutlineColor(isHovered ? accent : Colors::BORDER_DIM);
        window.draw(card);

        sf::RectangleShape topBar(sf::Vector2f(w, 6));
        topBar.setPosition(x, isHovered ? y - 10 : y);
        topBar.setFillColor(accent);
        window.draw(topBar);

        sf::Text tText;
        tText.setFont(font);
        tText.setString(title);
        tText.setCharacterSize(24);
        tText.setFillColor(Colors::TEXT_PRIMARY);
        tText.setStyle(sf::Text::Bold);
        tText.setPosition(x + 30, (isHovered ? y - 10 : y) + 40);
        window.draw(tText);

        sf::Text dText;
        dText.setFont(font);
        dText.setString(desc);
        dText.setCharacterSize(14);
        dText.setFillColor(Colors::TEXT_SECONDARY);
        dText.setPosition(x + 30, (isHovered ? y - 10 : y) + 90);
        
        // Simple word wrap
        std::string wrapped, line, word;
        for (char ch : desc) {
            if (ch == ' ') {
                if (line.length() + word.length() > 35) { wrapped += line + "\n"; line = word + " "; }
                else { line += word + " "; }
                word.clear();
            } else word += ch;
        }
        wrapped += line + word;
        dText.setString(wrapped);
        window.draw(dText);
    }
};