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

    void update(float deltaTime) {
        animTime += deltaTime;
    }

    void render(sf::RenderWindow& window) {
        // Gradient-like background using multiple rectangles
        drawBackground(window);

        if (!fontLoaded) return;

        // ============================================
        // ANIMATED TITLE
        // ============================================
        float pulseAlpha = 220 + std::sin(animTime * 2) * 35;
        sf::Color titleColor(230, 235, 245, (sf::Uint8)pulseAlpha);

        sf::Text title;
        title.setFont(font);
        title.setString("PATHFORGE");
        title.setCharacterSize(84);
        title.setFillColor(titleColor);
        title.setStyle(sf::Text::Bold);
        title.setLetterSpacing(1.3f);
        sf::FloatRect tb = title.getLocalBounds();
        title.setPosition((WINDOW_WIDTH - tb.width) / 2, 110);
        window.draw(title);

        // Accent underline (animated width)
        float underlineWidth = 300 + std::sin(animTime * 1.5f) * 40;
        sf::RectangleShape underline(sf::Vector2f(underlineWidth, 3));
        underline.setPosition((WINDOW_WIDTH - underlineWidth) / 2, 210);
        underline.setFillColor(Colors::TEXT_ACCENT);
        window.draw(underline);

        // ============================================
        // TAGLINE
        // ============================================
        sf::Text tagline;
        tagline.setFont(font);
        tagline.setString("Where algorithms come to life");
        tagline.setCharacterSize(22);
        tagline.setFillColor(Colors::TEXT_SECONDARY);
        tagline.setStyle(sf::Text::Italic);
        sf::FloatRect tgb = tagline.getLocalBounds();
        tagline.setPosition((WINDOW_WIDTH - tgb.width) / 2, 235);
        window.draw(tagline);

        // Small subtitle
        sf::Text subtitle;
        subtitle.setFont(font);
        subtitle.setString("A Pathfinding Visualization Engine  |  Built with C++ and SFML");
        subtitle.setCharacterSize(13);
        subtitle.setFillColor(sf::Color(120, 130, 150));
        sf::FloatRect sb = subtitle.getLocalBounds();
        subtitle.setPosition((WINDOW_WIDTH - sb.width) / 2, 275);
        window.draw(subtitle);

        // ============================================
        // SELECT MODE HEADING
        // ============================================
        sf::Text selectHeading;
        selectHeading.setFont(font);
        selectHeading.setString("CHOOSE YOUR MODE");
        selectHeading.setCharacterSize(14);
        selectHeading.setFillColor(Colors::TEXT_SECONDARY);
        selectHeading.setLetterSpacing(3.0f);
        sf::FloatRect shb = selectHeading.getLocalBounds();
        selectHeading.setPosition((WINDOW_WIDTH - shb.width) / 2, 340);
        window.draw(selectHeading);

        // ============================================
        // MODE CARDS
        // ============================================
        int cardWidth = 380;
        int cardHeight = 240;
        int cardY = 380;
        int gap = 50;
        int totalWidth = 2 * cardWidth + gap;
        int startX = (WINDOW_WIDTH - totalWidth) / 2;

        drawCard(window, startX, cardY, cardWidth, cardHeight,
                 sf::Color(255, 152, 0),
                 "1",
                 "CUSTOM MODE",
                 "Sculpt your own maze from scratch. Paint walls, place start and end points, and unleash algorithms on your creation.",
                 "* Full creative control\n* Draw walls with mouse\n* Test edge cases");

        drawCard(window, startX + cardWidth + gap, cardY, cardWidth, cardHeight,
                 sf::Color(46, 213, 115),
                 "2",
                 "PRESET MODE",
                 "Jump straight into action. 15 curated mazes across 3 difficulty tiers. Perfect for demos and quick benchmarking.",
                 "* 15 built-in mazes\n* Difficulty analysis\n* Instant loading");

        // ============================================
        // FEATURE HIGHLIGHTS
        // ============================================
        int featureY = 660;
        drawFeature(window, WINDOW_WIDTH / 4 - 100, featureY, "7+", "Algorithms");
        drawFeature(window, WINDOW_WIDTH / 2 - 60, featureY, "15", "Preset Mazes");
        drawFeature(window, 3 * WINDOW_WIDTH / 4 - 20, featureY, "AI", "Q-Learning");

        // ============================================
        // FOOTER
        // ============================================
        sf::Text footer;
        footer.setFont(font);
        footer.setString("Press 1 or 2 to begin  |  ESC to exit");
        footer.setCharacterSize(12);
        footer.setFillColor(sf::Color(100, 110, 130));
        sf::FloatRect fb = footer.getLocalBounds();
        footer.setPosition((WINDOW_WIDTH - fb.width) / 2, WINDOW_HEIGHT - 30);
        window.draw(footer);
    }

private:
    // Draw layered background for depth
    void drawBackground(sf::RenderWindow& window) {
        // Deep base
        sf::RectangleShape bg(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        bg.setFillColor(Colors::BG_DARK);
        window.draw(bg);

        // Top vignette (lighter)
        for (int i = 0; i < 100; i++) {
            sf::RectangleShape strip(sf::Vector2f(WINDOW_WIDTH, 2));
            strip.setPosition(0, i * 2);
            int alpha = 30 - (i / 4);
            if (alpha < 0) alpha = 0;
            strip.setFillColor(sf::Color(93, 173, 226, alpha));
            window.draw(strip);
        }

        // Bottom accent glow
        for (int i = 0; i < 60; i++) {
            sf::RectangleShape strip(sf::Vector2f(WINDOW_WIDTH, 3));
            strip.setPosition(0, WINDOW_HEIGHT - 180 + i * 3);
            int alpha = i / 3;
            strip.setFillColor(sf::Color(52, 152, 219, alpha));
            window.draw(strip);
        }

        // Subtle grid lines background (pathfinder theme)
        for (int i = 0; i < WINDOW_WIDTH; i += 40) {
            sf::RectangleShape line(sf::Vector2f(1, WINDOW_HEIGHT));
            line.setPosition(i, 0);
            line.setFillColor(sf::Color(255, 255, 255, 5));
            window.draw(line);
        }
        for (int i = 0; i < WINDOW_HEIGHT; i += 40) {
            sf::RectangleShape line(sf::Vector2f(WINDOW_WIDTH, 1));
            line.setPosition(0, i);
            line.setFillColor(sf::Color(255, 255, 255, 5));
            window.draw(line);
        }
    }

    void drawCard(sf::RenderWindow& window, int x, int y, int w, int h,
                  sf::Color accent, const std::string& keyText,
                  const std::string& title, const std::string& desc,
                  const std::string& features) {

        // Card shadow (offset dark rectangle)
        sf::RectangleShape shadow(sf::Vector2f(w, h));
        shadow.setPosition(x + 6, y + 6);
        shadow.setFillColor(sf::Color(0, 0, 0, 100));
        window.draw(shadow);

        // Card body
        sf::RectangleShape card(sf::Vector2f(w, h));
        card.setPosition(x, y);
        card.setFillColor(Colors::BG_MID);
        card.setOutlineThickness(2);
        card.setOutlineColor(accent);
        window.draw(card);

        // Top colored bar
        sf::RectangleShape topBar(sf::Vector2f(w, 6));
        topBar.setPosition(x, y);
        topBar.setFillColor(accent);
        window.draw(topBar);

        // Big key badge (circle-like with rectangle)
        sf::RectangleShape keyBadge(sf::Vector2f(60, 60));
        keyBadge.setPosition(x + 20, y + 25);
        keyBadge.setFillColor(accent);
        window.draw(keyBadge);

        sf::Text keyLabel;
        keyLabel.setFont(font);
        keyLabel.setString(keyText);
        keyLabel.setCharacterSize(38);
        keyLabel.setFillColor(Colors::TEXT_PRIMARY);
        keyLabel.setStyle(sf::Text::Bold);
        sf::FloatRect klb = keyLabel.getLocalBounds();
        keyLabel.setPosition(x + 20 + (60 - klb.width) / 2 - 2, y + 25 + 5);
        window.draw(keyLabel);

        // "PRESS" label above the badge
        sf::Text pressLabel;
        pressLabel.setFont(font);
        pressLabel.setString("PRESS");
        pressLabel.setCharacterSize(10);
        pressLabel.setFillColor(Colors::TEXT_SECONDARY);
        pressLabel.setLetterSpacing(2.0f);
        pressLabel.setPosition(x + 22, y + 12);
        window.draw(pressLabel);

        // Title
        sf::Text titleText;
        titleText.setFont(font);
        titleText.setString(title);
        titleText.setCharacterSize(22);
        titleText.setFillColor(Colors::TEXT_PRIMARY);
        titleText.setStyle(sf::Text::Bold);
        titleText.setPosition(x + 100, y + 35);
        window.draw(titleText);

        // Description with word wrap
        std::string wrapped = wordWrap(desc, 40);
        sf::Text descText;
        descText.setFont(font);
        descText.setString(wrapped);
        descText.setCharacterSize(12);
        descText.setFillColor(Colors::TEXT_SECONDARY);
        descText.setPosition(x + 100, y + 65);
        window.draw(descText);

        // Features list
        sf::Text featText;
        featText.setFont(font);
        featText.setString(features);
        featText.setCharacterSize(11);
        featText.setFillColor(sf::Color(140, 150, 170));
        featText.setPosition(x + 20, y + h - 70);
        window.draw(featText);
    }

    void drawFeature(sf::RenderWindow& window, int x, int y,
                     const std::string& number, const std::string& label) {
        // Big number
        sf::Text numText;
        numText.setFont(font);
        numText.setString(number);
        numText.setCharacterSize(40);
        numText.setFillColor(Colors::TEXT_ACCENT);
        numText.setStyle(sf::Text::Bold);
        numText.setPosition(x, y);
        window.draw(numText);

        // Label under
        sf::Text labText;
        labText.setFont(font);
        labText.setString(label);
        labText.setCharacterSize(12);
        labText.setFillColor(Colors::TEXT_SECONDARY);
        labText.setPosition(x, y + 50);
        window.draw(labText);
    }

    // Simple word wrap
    std::string wordWrap(const std::string& text, size_t width) {
        std::string result;
        std::string line;
        std::string word;
        for (char ch : text) {
            if (ch == ' ') {
                if (line.length() + word.length() > width) {
                    result += line + "\n";
                    line = word + " ";
                } else {
                    line += word + " ";
                }
                word.clear();
            } else {
                word += ch;
            }
        }
        line += word;
        result += line;
        return result;
    }
};