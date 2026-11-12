#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <memory>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>

#include "../core/Grid.h"
#include "../core/Constants.h"
#include "../rendering/Colors.h"
#include "../algorithms/Solver.h"

struct BenchmarkResult {
    std::string algo;
    bool pathFound = false;
    double timeMs = 0.0;
    int nodesExplored = 0;
    int pathLength = 0;
    int pathCost = 0;
};

enum class ChartMetric {
    TIME = 0,
    NODES = 1,
    LENGTH = 2,
    COST = 3
};

class BenchmarkScreen {
private:
    sf::Font m_font;
    bool m_fontLoaded = false;

    std::vector<BenchmarkResult> m_results;
    ChartMetric m_metric = ChartMetric::TIME;

    sf::RectangleShape m_overlay;
    sf::RectangleShape m_panel;
    
    // Bottom Buttons
    sf::RectangleShape m_closeButton;
    sf::Text m_closeText;
    bool m_closeHovered = false;

    sf::RectangleShape m_exportButton;
    sf::Text m_exportText;
    bool m_exportHovered = false;

    sf::Text m_statusMessageText;
    std::string m_statusMessage = "";

    sf::Text m_titleText;
    sf::Text m_subtitleText;

    static const int METRIC_COUNT = 4;
    sf::RectangleShape m_metricBtn[METRIC_COUNT];
    sf::Text m_metricText[METRIC_COUNT];
    bool m_metricHovered[METRIC_COUNT] = { false, false, false, false };

    const char* metricLabel(ChartMetric m) const {
        switch (m) {
            case ChartMetric::TIME:   return "Time (ms)";
            case ChartMetric::NODES:  return "Nodes";
            case ChartMetric::LENGTH: return "Length";
            case ChartMetric::COST:   return "Cost";
            default: return "";
        }
    }

    double metricValue(const BenchmarkResult& r, ChartMetric m) const {
        switch (m) {
            case ChartMetric::TIME:   return r.timeMs;
            case ChartMetric::NODES:  return static_cast<double>(r.nodesExplored);
            case ChartMetric::LENGTH: return static_cast<double>(r.pathLength);
            case ChartMetric::COST:   return static_cast<double>(r.pathCost);
            default: return 0.0;
        }
    }

    std::string formatMetric(double v, ChartMetric m) const {
        std::ostringstream oss;
        if (m == ChartMetric::TIME)
            oss << std::fixed << std::setprecision(3) << v;
        else
            oss << static_cast<long long>(v);
        return oss.str();
    }

    sf::Color barColor(size_t idx, bool found) const {
        if (!found) return sf::Color(100, 100, 110);
        if (idx == 0) return sf::Color(255, 215, 0); // Gold for #1
        static const sf::Color palette[] = {
            sf::Color(56, 189, 248),
            sf::Color(52, 211, 153),
            sf::Color(167, 139, 250),
            sf::Color(251, 146, 60),
            sf::Color(244, 114, 182),
            sf::Color(94, 234, 212),
            sf::Color(163, 230, 53)
        };
        return palette[idx % 7];
    }

    void layout() {
        m_overlay.setSize(sf::Vector2f(
            static_cast<float>(WINDOW_WIDTH),
            static_cast<float>(WINDOW_HEIGHT)
        ));
        m_overlay.setFillColor(sf::Color(15, 23, 42, 220));

        const float panelW = 900.f;
        const float panelH = 640.f;

        m_panel.setSize(sf::Vector2f(panelW, panelH));
        m_panel.setFillColor(Colors::BG_PANEL);
        m_panel.setOutlineColor(Colors::BORDER);
        m_panel.setOutlineThickness(2.f);
        m_panel.setPosition(
            (WINDOW_WIDTH - panelW) * 0.5f,
            (WINDOW_HEIGHT - panelH) * 0.5f
        );

        // Export CSV Button
        m_exportButton.setSize(sf::Vector2f(140.f, 40.f));
        m_exportButton.setPosition(
            m_panel.getPosition().x + panelW * 0.5f - 150.f,
            m_panel.getPosition().y + panelH - 52.f
        );

        // Close Button
        m_closeButton.setSize(sf::Vector2f(140.f, 40.f));
        m_closeButton.setPosition(
            m_panel.getPosition().x + panelW * 0.5f + 10.f,
            m_panel.getPosition().y + panelH - 52.f
        );

        // Metric tab buttons
        const float tabW = 110.f;
        const float tabH = 30.f;
        const float tabY = m_panel.getPosition().y + 325.f;
        float tabX = m_panel.getPosition().x + 28.f;

        const char* labels[METRIC_COUNT] = {
            "Time (ms)", "Nodes", "Length", "Cost"
        };

        for (int i = 0; i < METRIC_COUNT; ++i) {
            m_metricBtn[i].setSize(sf::Vector2f(tabW, tabH));
            m_metricBtn[i].setPosition(tabX + i * (tabW + 10.f), tabY);

            m_metricText[i].setString(labels[i]);
            m_metricText[i].setCharacterSize(13);
        }
    }

    void refreshButtonVisuals() {
        m_closeButton.setFillColor(
            m_closeHovered ? Colors::BUTTON_HOVER : Colors::BUTTON_BG
        );

        m_exportButton.setFillColor(
            m_exportHovered ? sf::Color(34, 197, 94) : sf::Color(21, 128, 61)
        );

        if (!m_fontLoaded) return;

        // Close Text
        m_closeText.setFont(m_font);
        m_closeText.setString("Close");
        m_closeText.setCharacterSize(15);
        m_closeText.setFillColor(Colors::TEXT_PRIMARY);

        sf::FloatRect tb = m_closeText.getLocalBounds();
        sf::FloatRect bb = m_closeButton.getGlobalBounds();
        m_closeText.setPosition(
            bb.left + (bb.width - tb.width) * 0.5f - tb.left,
            bb.top + (bb.height - tb.height) * 0.5f - tb.top - 1.f
        );

        // Export Text
        m_exportText.setFont(m_font);
        m_exportText.setString("Export CSV");
        m_exportText.setCharacterSize(15);
        m_exportText.setFillColor(Colors::TEXT_PRIMARY);

        sf::FloatRect eb = m_exportText.getLocalBounds();
        sf::FloatRect ebb = m_exportButton.getGlobalBounds();
        m_exportText.setPosition(
            ebb.left + (ebb.width - eb.width) * 0.5f - eb.left,
            ebb.top + (ebb.height - eb.height) * 0.5f - eb.top - 1.f
        );
    }

    void refreshMetricTabs() {
        if (!m_fontLoaded) return;

        for (int i = 0; i < METRIC_COUNT; ++i) {
            bool selected = (static_cast<int>(m_metric) == i);
            bool hovered = m_metricHovered[i];

            if (selected)
                m_metricBtn[i].setFillColor(sf::Color(37, 99, 235));
            else if (hovered)
                m_metricBtn[i].setFillColor(Colors::BUTTON_HOVER);
            else
                m_metricBtn[i].setFillColor(Colors::BUTTON_BG);

            m_metricText[i].setFont(m_font);
            m_metricText[i].setFillColor(Colors::TEXT_PRIMARY);

            sf::FloatRect tb = m_metricText[i].getLocalBounds();
            sf::FloatRect bb = m_metricBtn[i].getGlobalBounds();
            m_metricText[i].setPosition(
                bb.left + (bb.width - tb.width) * 0.5f - tb.left,
                bb.top + (bb.height - tb.height) * 0.5f - tb.top - 1.f
            );
        }
    }

    void exportCSV() {
        std::ofstream file("benchmark_results.csv");
        if (!file.is_open()) {
            m_statusMessage = "Failed to export CSV!";
            std::cerr << "Error: Unable to open benchmark_results.csv for writing." << std::endl;
            return;
        }

        // Header
        file << "Algorithm,Status,TimeMs,Nodes,Length,Cost\n";

        // Rows
        for (const auto& res : m_results) {
            file << res.algo << ","
                 << (res.pathFound ? "Found" : "Failed") << ","
                 << std::fixed << std::setprecision(3) << res.timeMs << ","
                 << res.nodesExplored << ","
                 << res.pathLength << ","
                 << res.pathCost << "\n";
        }

        file.close();
        m_statusMessage = "Exported to benchmark_results.csv!";
        std::cout << "[Benchmark] Successfully exported metrics to benchmark_results.csv" << std::endl;
    }

    void drawBarChart(sf::RenderWindow& window) {
        if (m_results.empty() || !m_fontLoaded) return;

        const sf::Vector2f pp = m_panel.getPosition();

        const float chartLeft   = pp.x + 28.f;
        const float chartTop    = pp.y + 368.f;
        const float chartWidth  = m_panel.getSize().x - 56.f;
        const float chartHeight = 160.f;
        const float labelColW   = 100.f;
        const float valueColW   = 80.f;
        const float barMaxW     = chartWidth - labelColW - valueColW - 16.f;

        // Dark background for chart region
        sf::RectangleShape chartBg(sf::Vector2f(chartWidth, chartHeight + 28.f));
        chartBg.setPosition(chartLeft, chartTop - 22.f);
        chartBg.setFillColor(sf::Color(15, 23, 42, 180));
        window.draw(chartBg);

        // Chart Title
        sf::Text chartTitle;
        chartTitle.setFont(m_font);
        chartTitle.setCharacterSize(14);
        chartTitle.setFillColor(Colors::TEXT_MUTED);
        chartTitle.setString(
            std::string("Comparison chart - ") + metricLabel(m_metric)
        );
        chartTitle.setPosition(chartLeft, chartTop - 20.f);
        window.draw(chartTitle);

        double maxVal = 0.0;
        for (const auto& r : m_results)
            maxVal = std::max(maxVal, metricValue(r, m_metric));
        if (maxVal <= 0.0) maxVal = 1.0;

        const size_t n = m_results.size();
        const float rowH = std::min(20.f, chartHeight / std::max<size_t>(1, n));
        float y = chartTop + 8.f;

        for (size_t i = 0; i < n; ++i) {
            const auto& res = m_results[i];
            double val = metricValue(res, m_metric);
            float barW = static_cast<float>((val / maxVal) * barMaxW);
            if (val > 0.0 && barW < 3.f) barW = 3.f;

            sf::Text name(res.algo, m_font, 13);
            name.setFillColor(Colors::TEXT_PRIMARY);
            name.setPosition(chartLeft, y);
            window.draw(name);

            sf::RectangleShape bar(sf::Vector2f(barW, rowH - 5.f));
            bar.setPosition(chartLeft + labelColW, y + 2.f);
            bar.setFillColor(barColor(i, res.pathFound));
            window.draw(bar);

            sf::Text valText(formatMetric(val, m_metric), m_font, 12);
            valText.setFillColor(Colors::TEXT_MUTED);
            valText.setPosition(chartLeft + labelColW + barW + 8.f, y);
            window.draw(valText);

            y += rowH;
        }
    }

public:
    BenchmarkScreen() {
        layout();
        refreshButtonVisuals();
        refreshMetricTabs();
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
            "Leaderboard + graphical comparison on the current maze"
        );
        m_subtitleText.setCharacterSize(14);
        m_subtitleText.setFillColor(Colors::TEXT_MUTED);

        m_statusMessageText.setFont(m_font);
        m_statusMessageText.setCharacterSize(13);
        m_statusMessageText.setFillColor(sf::Color(34, 197, 94)); // Green

        for (int i = 0; i < METRIC_COUNT; ++i)
            m_metricText[i].setFont(m_font);

        refreshButtonVisuals();
        refreshMetricTabs();
    }

    void runBenchmark(
        Grid& grid,
        const std::vector<std::string>& algos,
        std::function<std::unique_ptr<Solver>(const std::string&)> factory
    ) {
        m_results.clear();
        m_metric = ChartMetric::TIME;
        m_statusMessage = "";

        std::cout << "\n=================================\n";
        std::cout << "  BENCHMARK ALL - START\n";
        std::cout << "=================================\n";

        for (const auto& algo : algos) {
            grid.clearVisuals();
            auto solver = factory(algo);
            if (!solver) continue;

            auto t0 = std::chrono::steady_clock::now();
            while (!solver->isFinished())
                solver->step();
            auto t1 = std::chrono::steady_clock::now();

            auto us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

            BenchmarkResult res;
            res.algo = algo;
            res.pathFound = solver->hasPath();
            res.timeMs = us / 1000.0;
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

        std::sort(m_results.begin(), m_results.end(),
            [](const BenchmarkResult& a, const BenchmarkResult& b) {
                if (a.pathFound != b.pathFound)
                    return a.pathFound > b.pathFound;
                if (a.timeMs != b.timeMs)
                    return a.timeMs < b.timeMs;
                return a.nodesExplored < b.nodesExplored;
            });

        grid.clearVisuals();

        std::cout << "=================================\n";
        std::cout << "  BENCHMARK ALL - DONE\n";
        std::cout << "=================================\n\n";
    }

    bool handleClick(int mx, int my) {
        // Tab clicks
        for (int i = 0; i < METRIC_COUNT; ++i) {
            if (m_metricBtn[i].getGlobalBounds().contains(
                    static_cast<float>(mx), static_cast<float>(my))) {
                m_metric = static_cast<ChartMetric>(i);
                refreshMetricTabs();
                return false;
            }
        }

        // Export CSV Click
        if (m_exportButton.getGlobalBounds().contains(
                static_cast<float>(mx), static_cast<float>(my))) {
            exportCSV();
            return false; // Stay on screen
        }

        // Close Click
        return m_closeButton.getGlobalBounds().contains(
            static_cast<float>(mx), static_cast<float>(my)
        );
    }

    void updateHover(int mx, int my) {
        m_closeHovered = m_closeButton.getGlobalBounds().contains(
            static_cast<float>(mx), static_cast<float>(my)
        );

        m_exportHovered = m_exportButton.getGlobalBounds().contains(
            static_cast<float>(mx), static_cast<float>(my)
        );

        refreshButtonVisuals();

        for (int i = 0; i < METRIC_COUNT; ++i) {
            m_metricHovered[i] = m_metricBtn[i].getGlobalBounds().contains(
                static_cast<float>(mx), static_cast<float>(my)
            );
        }
        refreshMetricTabs();
    }

    void render(sf::RenderWindow& window) {
        window.draw(m_overlay);
        window.draw(m_panel);

        if (!m_fontLoaded) {
            window.draw(m_exportButton);
            window.draw(m_closeButton);
            return;
        }

        const sf::Vector2f pp = m_panel.getPosition();

        m_titleText.setPosition(pp.x + 28.f, pp.y + 16.f);
        window.draw(m_titleText);

        m_subtitleText.setPosition(pp.x + 28.f, pp.y + 46.f);
        window.draw(m_subtitleText);

        // -------- TABLE --------
        const float startX = pp.x + 28.f;
        const float startY = pp.y + 78.f;
        const float offsets[6] = { 0.f, 140.f, 240.f, 360.f, 480.f, 600.f };
        const char* headers[6] = {
            "Algorithm", "Status", "Time (ms)", "Nodes", "Length", "Cost"
        };

        for (int i = 0; i < 6; ++i) {
            sf::Text h(headers[i], m_font, 14);
            h.setFillColor(Colors::TEXT_MUTED);
            h.setPosition(startX + offsets[i], startY);
            window.draw(h);
        }

        sf::RectangleShape sep(sf::Vector2f(m_panel.getSize().x - 56.f, 1.f));
        sep.setFillColor(Colors::BORDER);
        sep.setPosition(startX, startY + 24.f);
        window.draw(sep);

        float rowY = startY + 34.f;
        for (size_t idx = 0; idx < m_results.size(); ++idx) {
            const auto& res = m_results[idx];

            sf::Color rowColor = Colors::TEXT_PRIMARY;
            if (idx == 0 && res.pathFound)
                rowColor = sf::Color(255, 215, 0);

            sf::Text tAlgo(res.algo, m_font, 14);
            tAlgo.setFillColor(rowColor);
            tAlgo.setPosition(startX + offsets[0], rowY);
            window.draw(tAlgo);

            sf::Text tStatus(res.pathFound ? "Found" : "Failed", m_font, 14);
            tStatus.setFillColor(
                res.pathFound ? sf::Color(34, 197, 94) : sf::Color(239, 68, 68)
            );
            tStatus.setPosition(startX + offsets[1], rowY);
            window.draw(tStatus);

            std::ostringstream oss;
            oss << std::fixed << std::setprecision(3) << res.timeMs;
            sf::Text tTime(oss.str(), m_font, 14);
            tTime.setFillColor(rowColor);
            tTime.setPosition(startX + offsets[2], rowY);
            window.draw(tTime);

            sf::Text tNodes(std::to_string(res.nodesExplored), m_font, 14);
            tNodes.setFillColor(rowColor);
            tNodes.setPosition(startX + offsets[3], rowY);
            window.draw(tNodes);

            sf::Text tLen(std::to_string(res.pathLength), m_font, 14);
            tLen.setFillColor(rowColor);
            tLen.setPosition(startX + offsets[4], rowY);
            window.draw(tLen);

            sf::Text tCost(std::to_string(res.pathCost), m_font, 14);
            tCost.setFillColor(rowColor);
            tCost.setPosition(startX + offsets[5], rowY);
            window.draw(tCost);

            rowY += 26.f;
        }

        // -------- METRIC TABS + BAR CHART --------
        for (int i = 0; i < METRIC_COUNT; ++i) {
            window.draw(m_metricBtn[i]);
            window.draw(m_metricText[i]);
        }

        drawBarChart(window);

        // -------- BOTTOM BUTTONS & STATUS MESSAGE --------
        window.draw(m_exportButton);
        window.draw(m_exportText);
        window.draw(m_closeButton);
        window.draw(m_closeText);

        if (!m_statusMessage.empty()) {
            m_statusMessageText.setString(m_statusMessage);
            sf::FloatRect sb = m_statusMessageText.getLocalBounds();
            m_statusMessageText.setPosition(
                pp.x + (m_panel.getSize().x - sb.width) * 0.5f,
                pp.y + m_panel.getSize().y - 82.f
            );
            window.draw(m_statusMessageText);
        }
    }
};