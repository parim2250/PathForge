#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include <memory>
#include <string>

#include "core/Grid.h"
#include "core/Constants.h"
#include "rendering/MazeRenderer.h"
#include "rendering/Colors.h"
#include "ui/InfoPanel.h"
#include "ui/StartupScreen.h"
#include "ui/PresetScreen.h"
#include "mazes/MazeLoader.h"

#include "algorithms/BFSSolver.h"
#include "algorithms/DFSSolver.h"
#include "algorithms/AStarSolver.h"
#include "algorithms/DijkstraSolver.h"
#include "algorithms/GreedySolver.h"
#include "algorithms/BidirectionalBFSSolver.h"
#include "algorithms/IDDFSSolver.h"

void mouseToGrid(int mouseX, int mouseY, int& row, int& col) {
    col = (mouseX - GRID_OFFSET_X) / CELL_SIZE;
    row = (mouseY - GRID_OFFSET_Y) / CELL_SIZE;
}

enum class AppState { STARTUP, PRESET_SELECT, RUNNING };

int main() {
    sf::RenderWindow window(
        sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
        "PathForge - Pathfinding Engine"
    );

    window.setFramerateLimit(60);

    Grid grid;
    MazeRenderer renderer;
    InfoPanel panel;
    StartupScreen startupScreen;
    PresetScreen presetScreen;

    panel.loadFont();
    startupScreen.loadFont();
    presetScreen.loadFont();

    AppState appState = AppState::STARTUP;

    bool isDragging = false;
    std::string modeLabel = "Custom Laboratory";

    std::unique_ptr<Solver> solver = nullptr;

    bool isSolving = false;
    bool isPaused = false;

    auto algorithmStartTime = std::chrono::steady_clock::now();
    auto lastStepTime = std::chrono::steady_clock::now();

    long long elapsedMs = 0;

    auto clearSolver = [&]() {
        solver = nullptr;
        isSolving = false;
        isPaused = false;

        panel.setRunning(false);
        panel.setPaused(false);
        panel.setMetrics(0, 0, 0, 0);

        elapsedMs = 0;
    };

    auto applyTool = [&](int row, int col) {
        if (!grid.inBounds(row, col)) return;
        if (isSolving) return;

        std::string tool = panel.getSelectedTool();

        if (tool == "Wall")      grid.setWall(row, col);
        else if (tool == "Erase") grid.setEmpty(row, col);
        else if (tool == "Start") grid.setStart(row, col);
        else if (tool == "End")   grid.setEnd(row, col);
        else if (tool == "Sand")  grid.setSand(row, col);
        else if (tool == "Water") grid.setWater(row, col);
        else if (tool == "Mud")   grid.setMud(row, col);
    };

    auto startAlgorithm = [&]() {
        if (!grid.hasStart() || !grid.hasEnd()) return;

        grid.clearVisuals();

        std::string algo = panel.getSelectedAlgo();

        if (algo == "BFS")
            solver = std::make_unique<BFSSolver>(&grid);
        else if (algo == "DFS")
            solver = std::make_unique<DFSSolver>(&grid);
        else if (algo == "A*")
            solver = std::make_unique<AStarSolver>(&grid);
        else if (algo == "Dijkstra")
            solver = std::make_unique<DijkstraSolver>(&grid);
        else if (algo == "Greedy")
            solver = std::make_unique<GreedySolver>(&grid);
        else if (algo == "BiDir BFS")
            solver = std::make_unique<BidirectionalBFSSolver>(&grid);
        else if (algo == "IDDFS")
            solver = std::make_unique<IDDFSSolver>(&grid);
        else
            return;

        isSolving = true;
        isPaused = false;

        panel.setRunning(true);
        panel.setPaused(false);

        algorithmStartTime = std::chrono::steady_clock::now();
        lastStepTime = algorithmStartTime;

        elapsedMs = 0;
        panel.setMetrics(0, 0, 0, 0);
    };

    sf::Font font;
    font.loadFromFile("C:/Windows/Fonts/segoeui.ttf");

    while (window.isOpen()) {
        sf::Event event;

        int mx = sf::Mouse::getPosition(window).x;
        int my = sf::Mouse::getPosition(window).y;

        panel.updateHover(mx, my);

        if (appState == AppState::PRESET_SELECT)
            presetScreen.updateHover(mx, my);

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape) {

                if (appState == AppState::RUNNING ||
                    appState == AppState::PRESET_SELECT) {

                    appState = AppState::STARTUP;

                    grid.clearAll();
                    clearSolver();

                    presetScreen.reset();
                    modeLabel = "Custom Laboratory";
                }
                else {
                    window.close();
                }
            }

            // ---------- STARTUP ----------
            if (appState == AppState::STARTUP) {
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {

                    int choice = startupScreen.checkClick();

                    if (choice == 1) {
                        grid.clearAll();
                        clearSolver();

                        modeLabel = "Custom Laboratory";
                        appState = AppState::RUNNING;
                    }
                    else if (choice == 2) {
                        presetScreen.reset();
                        appState = AppState::PRESET_SELECT;
                    }
                }

                continue;
            }

            // ---------- PRESET SELECT ----------
            if (appState == AppState::PRESET_SELECT) {
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {

                    int result = presetScreen.handleClick(mx, my);

                    // 11..15 represent levels 1..5.
                    if (result >= 11 && result <= 15) {
                        int level = result - 10;

                        Difficulty diff = presetScreen.getDifficulty();

                        grid.clearAll();
                        clearSolver();

                        MazeLoader::loadPreset(grid, diff, level);

                        auto info = PresetMazes::getInfo(diff, level);

                        std::string diffName =
                            MazeLoader::difficultyName(diff);

                        modeLabel =
                            "Preset / " + diffName +
                            " / Lv" + std::to_string(level) +
                            " - " + info.name;

                        appState = AppState::RUNNING;
                    }
                }

                continue;
            }

            // ---------- RUNNING ----------
            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {

                if (mx < MAZE_WIDTH) {
                    int row, col;

                    mouseToGrid(mx, my, row, col);

                    applyTool(row, col);
                    isDragging = true;
                }
                else {
                    std::string cmd = panel.handleClick(mx, my);

                    if (cmd == "CMD_RUN") {
                        startAlgorithm();
                    }
                    else if (cmd == "CMD_PAUSE") {
                        if (isSolving) {
                            isPaused = !isPaused;
                            panel.setPaused(isPaused);
                        }
                    }
                    else if (cmd == "CMD_RESET") {
                        grid.clearVisuals();
                        clearSolver();
                    }
                    else if (cmd == "TOOL_CHANGE" ||
                             cmd == "ALGO_CHANGE" ||
                             cmd == "SPEED_CHANGE") {
                        // State is already updated inside InfoPanel.
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased &&
                event.mouseButton.button == sf::Mouse::Left) {

                isDragging = false;
            }

            if (event.type == sf::Event::MouseMoved) {
                if (mx < MAZE_WIDTH) {
                    int row, col;

                    mouseToGrid(mx, my, row, col);

                    if (grid.inBounds(row, col)) {
                        renderer.setHover(row, col);

                        if (isDragging)
                            applyTool(row, col);
                    }
                    else {
                        renderer.clearHover();
                    }
                }
                else {
                    renderer.clearHover();
                }
            }
        }

        // ---------- ALGORITHM TICK ----------
        if (appState == AppState::RUNNING &&
            isSolving &&
            !isPaused &&
            solver &&
            !solver->isFinished()) {

            auto now = std::chrono::steady_clock::now();

            auto waitMs =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - lastStepTime
                ).count();

            if (waitMs >= panel.getSpeed()) {
                lastStepTime = now;

                // 1 ms = fastest, 200 ms = slowest.
                int speedMs = panel.getSpeed();
                int stepsPerTick;

                if (speedMs <= 1) {
                    stepsPerTick = 100000;
                }
                else if (speedMs <= 5) {
                    stepsPerTick = 2500;
                }
                else if (speedMs <= 10) {
                    stepsPerTick = 400;
                }
                else if (speedMs <= 20) {
                    stepsPerTick = 80;
                }
                else if (speedMs <= 40) {
                    stepsPerTick = 20;
                }
                else if (speedMs <= 80) {
                    stepsPerTick = 5;
                }
                else {
                    stepsPerTick = 1;
                }

                for (int i = 0; i < stepsPerTick; i++) {
                    if (solver->isFinished())
                        break;

                    solver->step();
                }

                // Capture time AFTER processing the batch.
                auto finishedAt = std::chrono::steady_clock::now();

                elapsedMs =
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        finishedAt - algorithmStartTime
                    ).count();

                panel.setMetrics(
                    solver->getNodesExplored(),
                    solver->getPathLength(),
                    solver->getPathCost(),
                    elapsedMs
                );

                if (solver->isFinished()) {
                    isSolving = false;
                    panel.setRunning(false);

                    int nodes = solver->getNodesExplored();
                    int len   = solver->getPathLength();
                    int cost  = solver->getPathCost();
                    bool ok   = solver->hasPath();

                    panel.setMetrics(nodes, len, cost, elapsedMs);

                    // Per-algorithm run report (one block every time you press Run)
                    std::cout << "  =================================" << std::endl;
                    std::cout << "  ALGORITHM RUN RESULT" << std::endl;
                    std::cout << "  =================================" << std::endl;
                    std::cout << "  Algorithm:      " << solver->getName() << std::endl;
                    std::cout << "  Path found:     " << (ok ? "YES" : "NO") << std::endl;
                    std::cout << "  Nodes explored: " << nodes << std::endl;
                    std::cout << "  Path length:    " << len << " cells" << std::endl;
                    std::cout << "  Path cost:      " << cost << std::endl;
                    std::cout << "  Exec time:      " << elapsedMs << " ms" << std::endl;
                    std::cout << "  =================================" << std::endl;
                    std::cout << std::endl;
                }
            }
        }
        else if (isSolving && isPaused) {
            algorithmStartTime +=
                std::chrono::steady_clock::now() - lastStepTime;

            lastStepTime = std::chrono::steady_clock::now();
        }

        // ---------- DRAW ----------
        window.clear(Colors::BG_APP);

        if (appState == AppState::STARTUP) {
            startupScreen.update(1.0f / 60.0f, mx, my);
            startupScreen.render(window);
        }
        else if (appState == AppState::PRESET_SELECT) {
            presetScreen.render(window);
        }
        else {
            renderer.render(window, grid);

            sf::RectangleShape navBg(
                sf::Vector2f(
                    static_cast<float>(MAZE_WIDTH),
                    static_cast<float>(TOP_NAV_HEIGHT)
                )
            );

            navBg.setFillColor(Colors::BG_APP);
            window.draw(navBg);

            sf::Text navText;

            navText.setFont(font);
            navText.setString("PATHFORGE   /   " + modeLabel);
            navText.setCharacterSize(14);
            navText.setFillColor(Colors::TEXT_PRIMARY);
            navText.setPosition(20.f, 20.f);

            window.draw(navText);

            panel.render(window);
        }

        window.display();
    }

    return 0;
}