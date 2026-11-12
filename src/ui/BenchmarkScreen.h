#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <memory>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

#include "../core/Grid.h"
#include "../core/Constants.h"
#include "../rendering/Colors.h"
#include "../algorithms/Solver.h"

struct BenchmarkResult {
    std::string algo;
    bool pathFound = false;
    double timeMs = 0.0;   // fractional milliseconds
    int nodesExplored = 0;
    int pathLength = 0;
    int pathCost = 0;
};

class BenchmarkScreen {
private:
    sf::Font m_font;
    bool m_fontLoaded = false;

    std::vector<BenchmarkResult> m_results;

    sf::RectangleShape m_overlay;
    sf::RectangleShape m_panel;
    sf::RectangleShape m_closeButton;
    sf::Text m_closeText;
    sf::Text m_titleText;
    sf::Text m_subtitleText;

    bool m_closeHovered = false;

    void layout() {
        m_overlay.setSize(sf::Vector2f(
            static_cast<float>(WINDOW_WIDTH),
            static_cast<float>(WINDOW_HEIGHT)
        ));
        m_overlay.setFillColor(sf::Color(15, 23, 42, 220));

        const float panelW = 860.f;
        const float panelH = 520.f;

        m_panel.setSize(sf::Vector2f(panelW, panelH));
        m_panel.setFillColor(Colors::BG_PANEL);
        m_panel.setOutlineColor(Colors::BORDER);
        m_panel.setOutlineThickness(2.f);
        m_panel.setPosition(
            (WINDOW_WIDTH - panelW) * 0.5f,
            (WINDOW_HEIGHT - panelH) * 0.5f
        );

        m_closeButton.setSize(sf::Vector2f(140.f, 42.f));
        m_closeButton.setPosition(
            m_panel.getPosition().x + panelW * 0.5f - 70.f,
            m_panel.getPosition().y + panelH - 64.f
        );
    }

    void refreshCloseVisual() {
        m_closeButton.setFillColor(
            m_closeHovered ? Colors::BUTTON_HOVER : Colors::BUTTON_BG
        );

        if (!m_fontLoaded)
            return;

        m_closeText.setFont(m_font);
        m_closeText.setString("Close");
        m_closeText.setCharacterSize(16);
        m_closeText.setFillColor(Colors::TEXT_PRIMARY);

        sf::FloatRect tb = m_closeText.getLocalBounds();
        sf::FloatRect bb = m_closeButton.getGlobalBounds();

        m_closeText.setPosition(
            bb.left + (bb.width - tb.width) * 0.5f - tb.left,
            bb.top + (bb.height - tb.height) * 0.5f - tb.top - 1.f
        );
    }

public:
    BenchmarkScreen() {
        layout();
        refreshCloseVisual();
    }

    void loadFont() {
        m_fontLoaded = m_font.loadFromFile("C:/Windows/Fonts/segoeui.ttf");
        if (!m_fontLoaded) {
            std::cerr << "BenchmarkScreen: failed to load font\n";
            return;
        }

        m_titleText.setFont(m_font);
        m_titleText.setString("BENCHMARK RESULTS");
        m_titleText.setCharacterSize(24);
        m_titleText.setStyle(sf::Text::Bold);
        m_titleText.setFillColor(Colors::TEXT_PRIMARY);

        m_subtitleText.setFont(m_font);
        m_subtitleText.setString(
            "All algorithms ran in backend on the current maze"
        );
        m_subtitleText.setCharacterSize(14);
        m_subtitleText.setFillColor(Colors::TEXT_MUTED);

        refreshCloseVisual();
    }

    void runBenchmark(
        Grid& grid,
        const std::vector<std::string>& algos,
        std::function<std::unique_ptr<Solver>(const std::string&)> factory
    ) {
        m_results.clear();

        std::cout << "\n=================================\n";
        std::cout << "  BENCHMARK ALL - START\n";
        std::cout << "=================================\n";

        for (const auto& algo : algos) {
            grid.clearVisuals();

            auto solver = factory(algo);
            if (!solver)
                continue;

            auto t0 = std::chrono::steady_clock::now();

            while (!solver->isFinished())
                solver->step();

            auto t1 = std::chrono::steady_clock::now();
            auto us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
            double ms = us / 1000.0;

            BenchmarkResult res;
            res.algo = algo;
            res.pathFound = solver->hasPath();
            res.timeMs = ms;
            res.nodesExplored = solver->getNodesExplored();
            res.pathLength = solver->getPathLength();
            res.pathCost = solver->getPathCost();
            m_results.push_back(res);

            std::cout << "  " << algo
                      << " | found=" << (res.pathFound ? "Y" : "N")
                      << " | time=" << std::fixed << std::setprecision(3) << res.timeMs << "ms"
                      << " | nodes=" << res.nodesExplored
                      << " | len=" << res.pathLength
                      << " | cost=" << res.pathCost
                      << "\n";
        }

        // Rank by: path found first, then lower time, then fewer nodes.
        std::sort(m_results.begin(), m_results.end(),
            [](const BenchmarkResult& a, const BenchmarkResult& b) {
                if (a.pathFound != b.pathFound)
                    return a.pathFound > b.pathFound;
                if (a.timeMs != b.timeMs)
                    return a.timeMs < b.timeMs;
                return a.nodesExplored < b.nodesExplored;
            }
        );

        grid.clearVisuals();

        std::cout << "=================================\n";
        std::cout << "  BENCHMARK ALL - DONE\n";
        std::cout << "=================================\n\n";
    }

    bool handleClick(int mx, int my) {
        return m_closeButton.getGlobalBounds().contains(
            static_cast<float>(mx),
            static_cast<float>(my)
        );
    }

    void updateHover(int mx, int my) {
        m_closeHovered = m_closeButton.getGlobalBounds().contains(
            static_cast<float>(mx),
            static_cast<float>(my)
        );
        refreshCloseVisual();
    }

    void render(sf::RenderWindow& window) {
        window.draw(m_overlay);
        window.draw(m_panel);

        if (!m_fontLoaded) {
            window.draw(m_closeButton);
            return;
        }

        const sf::Vector2f pp = m_panel.getPosition();

        m_titleText.setPosition(pp.x + 28.f, pp.y + 18.f);
        window.draw(m_titleText);

        m_subtitleText.setPosition(pp.x + 28.f, pp.y + 50.f);
        window.draw(m_subtitleText);

        const float startX = pp.x + 28.f;
        const float startY = pp.y + 92.f;
        const float offsets[6] = { 0.f, 150.f, 260.f, 380.f, 500.f, 620.f };
        const char* headers[6] = {
            "Algorithm", "Status", "Time (ms)", "Nodes", "Length", "Cost"
        };

        for (int i = 0; i < 6; ++i) {
            sf::Text h(headers[i], m_font, 15);
            h.setFillColor(Colors::TEXT_MUTED);
            h.setPosition(startX + offsets[i], startY);
            window.draw(h);
        }

        sf::RectangleShape sep(sf::Vector2f(m_panel.getSize().x - 56.f, 1.f));
        sep.setFillColor(Colors::BORDER);
        sep.setPosition(startX, startY + 28.f);
        window.draw(sep);

        float rowY = startY + 42.f;

        for (size_t idx = 0; idx < m_results.size(); ++idx) {
            const auto& res = m_results[idx];

            // Gold tint for rank #1 when a path exists.
            sf::Color rowColor = Colors::TEXT_PRIMARY;
            if (idx == 0 && res.pathFound)
                rowColor = sf::Color(255, 215, 0);

            sf::Text tAlgo(res.algo, m_font, 16);
            tAlgo.setFillColor(rowColor);
            tAlgo.setPosition(startX + offsets[0], rowY);
            window.draw(tAlgo);

            sf::Text tStatus(res.pathFound ? "Found" : "Failed", m_font, 16);
            tStatus.setFillColor(
                res.pathFound ? sf::Color(34, 197, 94)
                              : sf::Color(239, 68, 68)
            );
            tStatus.setPosition(startX + offsets[1], rowY);
            window.draw(tStatus);

            std::ostringstream oss;
            oss << std::fixed << std::setprecision(3) << res.timeMs;
            sf::Text tTime(oss.str(), m_font, 16);
            tTime.setFillColor(rowColor);
            tTime.setPosition(startX + offsets[2], rowY);
            window.draw(tTime);

            sf::Text tNodes(std::to_string(res.nodesExplored), m_font, 16);
            tNodes.setFillColor(rowColor);
            tNodes.setPosition(startX + offsets[3], rowY);
            window.draw(tNodes);

            sf::Text tLen(std::to_string(res.pathLength), m_font, 16);
            tLen.setFillColor(rowColor);
            tLen.setPosition(startX + offsets[4], rowY);
            window.draw(tLen);

            sf::Text tCost(std::to_string(res.pathCost), m_font, 16);
            tCost.setFillColor(rowColor);
            tCost.setPosition(startX + offsets[5], rowY);
            window.draw(tCost);

            rowY += 36.f;
        }

        window.draw(m_closeButton);
        window.draw(m_closeText);
    }
};