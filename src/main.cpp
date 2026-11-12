#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <functional>

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

// Include after solver headers so Solver is declared.
#include "ui/BenchmarkScreen.h"

void mouseToGrid(int mouseX, int mouseY, int& row, int& col) {
    col = (mouseX - GRID_OFFSET_X) / CELL_SIZE;
    row = (mouseY - GRID_OFFSET_Y) / CELL_SIZE;
}

enum class AppState {
    STARTUP,
    PRESET_SELECT,
    RUNNING,
    BENCHMARK
};

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
    BenchmarkScreen benchmarkScreen;

    panel.loadFont();
    startupScreen.loadFont();
    presetScreen.loadFont();
    benchmarkScreen.loadFont();

    AppState appState = AppState::STARTUP;

    bool isDragging = false;
    std::string modeLabel = "Custom Laboratory";

    // Two solvers support normal mode and Race Mode.
    std::unique_ptr<Solver> solverA = nullptr;
    std::unique_ptr<Solver> solverB = nullptr;

    bool isSolvingA = false;
    bool isSolvingB = false;
    bool isPaused = false;

    auto algorithmStartTime = std::chrono::steady_clock::now();
    auto lastStepTime = std::chrono::steady_clock::now();

    long long elapsedMsA = 0;
    long long elapsedMsB = 0;

    // --------------------------------------------------
    // SOLVER FACTORY
    // --------------------------------------------------

    auto createSolver = [&](const std::string& algo)
        -> std::unique_ptr<Solver> {

        if (algo == "BFS")
            return std::make_unique<BFSSolver>(&grid);

        if (algo == "DFS")
            return std::make_unique<DFSSolver>(&grid);

        if (algo == "A*")
            return std::make_unique<AStarSolver>(&grid);

        if (algo == "Dijkstra")
            return std::make_unique<DijkstraSolver>(&grid);

        if (algo == "Greedy")
            return std::make_unique<GreedySolver>(&grid);

        if (algo == "BiDir BFS")
            return std::make_unique<BidirectionalBFSSolver>(&grid);

        if (algo == "IDDFS")
            return std::make_unique<IDDFSSolver>(&grid);

        return nullptr;
    };

    // --------------------------------------------------
    // CLEAR SOLVERS
    // --------------------------------------------------

    auto clearSolver = [&]() {

        solverA = nullptr;
        solverB = nullptr;

        isSolvingA = false;
        isSolvingB = false;
        isPaused = false;

        panel.setRunning(false);
        panel.setPaused(false);

        panel.setMetrics(0, 0, 0, 0);
        panel.setMetricsB(0, 0, 0, 0);

        elapsedMsA = 0;
        elapsedMsB = 0;
    };

    // --------------------------------------------------
    // APPLY EDITOR TOOLS
    // --------------------------------------------------

    auto applyTool = [&](int row, int col) {

        if (!grid.inBounds(row, col))
            return;

        if (isSolvingA || isSolvingB)
            return;

        std::string tool = panel.getSelectedTool();

        if (tool == "Wall")
            grid.setWall(row, col);

        else if (tool == "Erase")
            grid.setEmpty(row, col);

        else if (tool == "Start")
            grid.setStart(row, col);

        else if (tool == "End")
            grid.setEnd(row, col);

        else if (tool == "Sand")
            grid.setSand(row, col);

        else if (tool == "Water")
            grid.setWater(row, col);

        else if (tool == "Mud")
            grid.setMud(row, col);
    };

    // --------------------------------------------------
    // PRINT ALGORITHM REPORT
    // --------------------------------------------------

    auto printReport = [&](Solver* solver, long long elapsedMs) {

        if (!solver)
            return;

        std::cout << "\n  =================================" << std::endl;
        std::cout << "  ALGORITHM RUN RESULT" << std::endl;
        std::cout << "  =================================" << std::endl;

        std::cout << "  Algorithm:      "
                  << solver->getName() << std::endl;

        std::cout << "  Path found:     "
                  << (solver->hasPath() ? "YES" : "NO") << std::endl;

        std::cout << "  Nodes explored: "
                  << solver->getNodesExplored() << std::endl;

        std::cout << "  Path length:    "
                  << solver->getPathLength() << " cells" << std::endl;

        std::cout << "  Path cost:      "
                  << solver->getPathCost() << std::endl;

        std::cout << "  Exec time:      "
                  << elapsedMs << " ms" << std::endl;

        std::cout << "  =================================\n" << std::endl;
    };

    // --------------------------------------------------
    // START ALGORITHM / RACE
    // --------------------------------------------------

    auto startAlgorithm = [&]() {

        if (!grid.hasStart() || !grid.hasEnd())
            return;

        grid.clearVisuals();
        clearSolver();

        // Solver A: selected algorithm.
        std::string algoA = panel.getMode() == PanelMode::RACE
            ? panel.getSelectedAlgoA()
            : panel.getSelectedAlgo();

        solverA = createSolver(algoA);

        if (solverA)
            isSolvingA = true;

        // Solver B: only created in Race Mode.
        if (panel.getMode() == PanelMode::RACE) {

            std::string algoB = panel.getSelectedAlgoB();

            solverB = createSolver(algoB);

            if (solverB)
                isSolvingB = true;
        }

        if (!isSolvingA && !isSolvingB)
            return;

        isPaused = false;

        panel.setRunning(true);
        panel.setPaused(false);

        algorithmStartTime = std::chrono::steady_clock::now();
        lastStepTime = algorithmStartTime;

        elapsedMsA = 0;
        elapsedMsB = 0;
    };

    // --------------------------------------------------
    // FONT FOR NAVIGATION
    // --------------------------------------------------

    sf::Font font;
    font.loadFromFile("C:/Windows/Fonts/segoeui.ttf");

    // --------------------------------------------------
    // MAIN LOOP
    // --------------------------------------------------

    while (window.isOpen()) {

        sf::Event event;

        int mx = sf::Mouse::getPosition(window).x;
        int my = sf::Mouse::getPosition(window).y;

        if (appState == AppState::RUNNING)
            panel.updateHover(mx, my);

        if (appState == AppState::PRESET_SELECT)
            presetScreen.updateHover(mx, my);

        if (appState == AppState::BENCHMARK)
            benchmarkScreen.updateHover(mx, my);

        // --------------------------------------------------
        // EVENT HANDLING
        // --------------------------------------------------

        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();

            // ---------------- ESCAPE ----------------

            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape) {

                if (appState == AppState::BENCHMARK) {

                    appState = AppState::RUNNING;
                }

                else if (appState == AppState::RUNNING ||
                         appState == AppState::PRESET_SELECT) {

                    appState = AppState::STARTUP;

                    grid.clearAll();
                    clearSolver();

                    presetScreen.reset();

                    modeLabel = "Custom Laboratory";
                    isDragging = false;
                }

                else {
                    window.close();
                }
            }

            // ---------------- STARTUP ----------------

            if (appState == AppState::STARTUP) {

                if (event.type == sf::Event::MouseMoved) {

                    startupScreen.update(
                        0.0f,
                        event.mouseMove.x,
                        event.mouseMove.y
                    );
                }

                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {

                    startupScreen.update(
                        0.0f,
                        event.mouseButton.x,
                        event.mouseButton.y
                    );

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

            // ---------------- PRESET SELECT ----------------

            if (appState == AppState::PRESET_SELECT) {

                if (event.type == sf::Event::MouseMoved) {

                    presetScreen.updateHover(
                        event.mouseMove.x,
                        event.mouseMove.y
                    );
                }

                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {

                    int clickX = event.mouseButton.x;
                    int clickY = event.mouseButton.y;

                    presetScreen.updateHover(clickX, clickY);

                    int result = presetScreen.handleClick(clickX, clickY);

                    // 11..15 represent levels 1..5.
                    if (result >= 11 && result <= 15) {

                        int level = result - 10;

                        Difficulty diff = presetScreen.getDifficulty();

                        grid.clearAll();
                        clearSolver();

                        bool loaded = MazeLoader::loadPreset(
                            grid,
                            diff,
                            level
                        );

                        if (loaded) {

                            auto info = PresetMazes::getInfo(diff, level);

                            std::string diffName =
                                MazeLoader::difficultyName(diff);

                            modeLabel =
                                "Preset / " + diffName +
                                " / Lv" + std::to_string(level) +
                                " - " + info.name;

                            appState = AppState::RUNNING;
                        }
                        else {

                            grid.clearAll();

                            std::cerr
                                << "Preset loading failed. "
                                << "Please select another level."
                                << std::endl;
                        }
                    }
                }

                continue;
            }

            // ---------------- BENCHMARK SCREEN ----------------

            if (appState == AppState::BENCHMARK) {

                if (event.type == sf::Event::MouseMoved) {

                    benchmarkScreen.updateHover(
                        event.mouseMove.x,
                        event.mouseMove.y
                    );
                }

                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {

                    if (benchmarkScreen.handleClick(
                            event.mouseButton.x,
                            event.mouseButton.y)) {

                        appState = AppState::RUNNING;
                    }
                }

                continue;
            }

            // ---------------- RUNNING ----------------

            if (appState == AppState::RUNNING) {

                // Mouse click.
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {

                    int clickX = event.mouseButton.x;
                    int clickY = event.mouseButton.y;

                    if (clickX < MAZE_WIDTH) {

                        int row, col;

                        mouseToGrid(clickX, clickY, row, col);

                        applyTool(row, col);

                        isDragging = true;
                    }

                    else {

                        panel.updateHover(clickX, clickY);

                        std::string cmd = panel.handleClick(
                            clickX,
                            clickY
                        );

                        if (cmd == "CMD_RUN") {

                            startAlgorithm();
                        }

                        else if (cmd == "CMD_PAUSE") {

                            if (isSolvingA || isSolvingB) {

                                isPaused = !isPaused;

                                panel.setPaused(isPaused);
                            }
                        }

                        else if (cmd == "CMD_RESET") {

                            grid.clearVisuals();
                            clearSolver();
                        }

                        else if (cmd == "CMD_RACE_ALL") {

                            if (grid.hasStart() && grid.hasEnd()) {

                                grid.clearVisuals();
                                clearSolver();

                                std::vector<std::string> allAlgos = {
                                    "BFS",
                                    "DFS",
                                    "A*",
                                    "Dijkstra",
                                    "Greedy",
                                    "BiDir BFS",
                                    "IDDFS"
                                };

                                benchmarkScreen.runBenchmark(
                                    grid,
                                    allAlgos,
                                    createSolver
                                );

                                appState = AppState::BENCHMARK;
                            }
                        }

                        // TOOL_CHANGE, ALGO_CHANGE and SPEED_CHANGE
                        // are already handled by InfoPanel.
                    }
                }

                // Mouse release.
                if (event.type == sf::Event::MouseButtonReleased &&
                    event.mouseButton.button == sf::Mouse::Left) {

                    isDragging = false;
                }

                // Mouse movement and drag painting.
                if (event.type == sf::Event::MouseMoved) {

                    int moveX = event.mouseMove.x;
                    int moveY = event.mouseMove.y;

                    if (moveX < MAZE_WIDTH) {

                        int row, col;

                        mouseToGrid(moveX, moveY, row, col);

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
                        panel.updateHover(moveX, moveY);
                    }
                }
            }
        }

        // --------------------------------------------------
        // ALGORITHM TICK
        // --------------------------------------------------

        bool solvingAny = isSolvingA || isSolvingB;

        if (appState == AppState::RUNNING &&
            solvingAny &&
            !isPaused) {

            auto now = std::chrono::steady_clock::now();

            auto waitMs =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - lastStepTime
                ).count();

            if (waitMs >= panel.getSpeed()) {

                lastStepTime = now;

                int speedMs = panel.getSpeed();
                int stepsPerTick;

                if (speedMs <= 1)
                    stepsPerTick = 100000;

                else if (speedMs <= 5)
                    stepsPerTick = 2500;

                else if (speedMs <= 10)
                    stepsPerTick = 400;

                else if (speedMs <= 20)
                    stepsPerTick = 80;

                else if (speedMs <= 40)
                    stepsPerTick = 20;

                else if (speedMs <= 80)
                    stepsPerTick = 5;

                else
                    stepsPerTick = 1;

                // Step both solvers equally for a fair race.
                for (int i = 0; i < stepsPerTick; i++) {

                    if (isSolvingA &&
                        solverA &&
                        !solverA->isFinished()) {

                        solverA->step();
                    }

                    if (isSolvingB &&
                        solverB &&
                        !solverB->isFinished()) {

                        solverB->step();
                    }

                    bool doneA = !isSolvingA ||
                                 !solverA ||
                                 solverA->isFinished();

                    bool doneB = !isSolvingB ||
                                 !solverB ||
                                 solverB->isFinished();

                    if (doneA && doneB)
                        break;
                }

                auto finishedAt = std::chrono::steady_clock::now();

                // ---------------- SOLVER A ----------------

                if (isSolvingA && solverA) {

                    elapsedMsA =
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            finishedAt - algorithmStartTime
                        ).count();

                    panel.setMetrics(
                        solverA->getNodesExplored(),
                        solverA->getPathLength(),
                        solverA->getPathCost(),
                        elapsedMsA
                    );

                    if (solverA->isFinished()) {

                        isSolvingA = false;

                        printReport(
                            solverA.get(),
                            elapsedMsA
                        );
                    }
                }

                // ---------------- SOLVER B ----------------

                if (isSolvingB && solverB) {

                    elapsedMsB =
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            finishedAt - algorithmStartTime
                        ).count();

                    panel.setMetricsB(
                        solverB->getNodesExplored(),
                        solverB->getPathLength(),
                        solverB->getPathCost(),
                        elapsedMsB
                    );

                    if (solverB->isFinished()) {

                        isSolvingB = false;

                        printReport(
                            solverB.get(),
                            elapsedMsB
                        );
                    }
                }

                // Both algorithms have finished.
                if (!isSolvingA && !isSolvingB) {

                    panel.setRunning(false);
                }
            }
        }

        // --------------------------------------------------
        // PAUSE TIMER HANDLING
        // --------------------------------------------------

        else if (solvingAny && isPaused) {

            auto now = std::chrono::steady_clock::now();

            algorithmStartTime += now - lastStepTime;

            lastStepTime = now;
        }

        // --------------------------------------------------
        // DRAW
        // --------------------------------------------------

        window.clear(Colors::BG_APP);

        if (appState == AppState::STARTUP) {

            startupScreen.update(
                1.0f / 60.0f,
                mx,
                my
            );

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
            navText.setString(
                "PATHFORGE   /   " + modeLabel
            );

            navText.setCharacterSize(14);
            navText.setFillColor(Colors::TEXT_PRIMARY);
            navText.setPosition(20.f, 20.f);

            window.draw(navText);

            panel.render(window);

            // Benchmark overlay.
            if (appState == AppState::BENCHMARK) {

                benchmarkScreen.render(window);
            }
        }

        window.display();
    }

    return 0;
}