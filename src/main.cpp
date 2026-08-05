#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include <memory>
#include "core/Grid.h"
#include "core/Constants.h"
#include "rendering/MazeRenderer.h"
#include "rendering/Colors.h"
#include "ui/InfoPanel.h"
#include "ui/StartupScreen.h"
#include "mazes/MazeLoader.h"
#include "algorithms/BFSSolver.h"
#include "algorithms/DFSSolver.h"
#include "algorithms/AStarSolver.h"
#include "algorithms/DijkstraSolver.h"
#include "algorithms/GreedySolver.h"
#include "algorithms/BidirectionalBFSSolver.h"
#include "algorithms/IDDFSSolver.h"

void mouseToGrid(int mouseX, int mouseY, int& row, int& col) {
    col = mouseX / CELL_SIZE;
    row = mouseY / CELL_SIZE;
}

enum class PlaceMode { WALL, START, END, ERASE, SAND, WATER, MUD };
enum class AppState { STARTUP, RUNNING };

std::string modeToString(PlaceMode mode) {
    switch (mode) {
        case PlaceMode::WALL:  return "WALL";
        case PlaceMode::START: return "START";
        case PlaceMode::END:   return "END";
        case PlaceMode::ERASE: return "ERASE";
        case PlaceMode::SAND:  return "SAND";
        case PlaceMode::WATER: return "WATER";
        case PlaceMode::MUD:   return "MUD";
    }
    return "WALL";
}

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

    if (!panel.loadFont()) std::cout << "WARNING: Panel font missing" << std::endl;
    if (!startupScreen.loadFont()) std::cout << "WARNING: Startup font missing" << std::endl;

    AppState appState = AppState::STARTUP;
    GameMode gameMode = GameMode::CUSTOM;
    PlaceMode currentMode = PlaceMode::WALL;
    Difficulty currentDifficulty = Difficulty::BEGINNER;
    int currentLevel = 1;
    bool isDragging = false;

    // Algorithm solvers
    std::unique_ptr<Solver> solver1 = nullptr;
    std::unique_ptr<Solver> solver2 = nullptr;
    bool isSolving = false;

    // Animation timing
    int animationSpeed = 20;
    auto lastStepTime = std::chrono::steady_clock::now();
    auto algorithmStartTime = std::chrono::steady_clock::now();

    std::cout << "===================================" << std::endl;
    std::cout << "  PathForge - Part 3 Ready!" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "CUSTOM MODE keys:" << std::endl;
    std::cout << "  1=BFS 2=DFS 3=Race(BFS+DFS)" << std::endl;
    std::cout << "  4=A* 5=Dijkstra 6=Greedy" << std::endl;
    std::cout << "  7=BiDir BFS 8=IDDFS 9=A* vs BFS" << std::endl;
    std::cout << std::endl;
    std::cout << "PRESET MODE keys:" << std::endl;
    std::cout << "  1-5=Load level, Q=BFS E=DFS R=Race" << std::endl;
    std::cout << "  T=A* Y=Dijkstra U=Greedy O=BiDir P=IDDFS Z=A* vs BFS" << std::endl;
    std::cout << std::endl;
    std::cout << "Press +/- for speed, G to clear paths, C to clear all" << std::endl;

   auto applyMode = [&](int row, int col) {
    if (!grid.inBounds(row, col)) return;
    switch (currentMode) {
        case PlaceMode::WALL:  grid.setWall(row, col); break;
        case PlaceMode::START: grid.setStart(row, col); break;
        case PlaceMode::END:   grid.setEnd(row, col); break;
        case PlaceMode::ERASE: grid.setEmpty(row, col); break;
        case PlaceMode::SAND:  grid.setSand(row, col); break;
        case PlaceMode::WATER: grid.setWater(row, col); break;
        case PlaceMode::MUD:   grid.setMud(row, col); break;
    }
};

    auto loadPreset = [&](Difficulty diff, int level) {
        MazeLoader::loadPreset(grid, diff, level);
        auto info = PresetMazes::getInfo(diff, level);
        currentDifficulty = diff;
        currentLevel = level;
        panel.setPresetInfo(MazeLoader::difficultyName(diff), level, info.name);
        std::cout << "\n>>> Loaded: " << MazeLoader::difficultyName(diff)
                  << " L" << level << " - " << info.name << std::endl;
    };

    // ============================================
    // ALGORITHM STARTERS
    // ============================================
    auto startAlgo = [&](std::unique_ptr<Solver> solver, const std::string& name) {
        if (!grid.hasStart() || !grid.hasEnd()) {
            std::cout << "\n[ERROR] Place both START and END first!" << std::endl;
            return;
        }
        grid.clearVisuals();
        solver1 = std::move(solver);
        solver2 = nullptr;
        isSolving = true;
        algorithmStartTime = std::chrono::steady_clock::now();
        panel.setAlgo1(name, 0, 0, false, true);
        panel.setAlgo2("", 0, 0, false, false);
        std::cout << "\n>>> Running " << name << "..." << std::endl;
    };

    auto startBFS      = [&]() { startAlgo(std::make_unique<BFSSolver>(&grid), "BFS"); };
    auto startDFS      = [&]() { startAlgo(std::make_unique<DFSSolver>(&grid), "DFS"); };
    auto startAStar    = [&]() { startAlgo(std::make_unique<AStarSolver>(&grid), "A*"); };
    auto startDijkstra = [&]() { startAlgo(std::make_unique<DijkstraSolver>(&grid), "Dijkstra"); };
    auto startGreedy   = [&]() { startAlgo(std::make_unique<GreedySolver>(&grid), "Greedy"); };
    auto startBidir    = [&]() { startAlgo(std::make_unique<BidirectionalBFSSolver>(&grid), "BiDir BFS"); };
    auto startIDDFS    = [&]() { startAlgo(std::make_unique<IDDFSSolver>(&grid), "IDDFS"); };

    // Race any two algorithms
    auto startRace = [&](std::unique_ptr<Solver> s1, std::unique_ptr<Solver> s2,
                          const std::string& n1, const std::string& n2) {
        if (!grid.hasStart() || !grid.hasEnd()) {
            std::cout << "\n[ERROR] Place both START and END first!" << std::endl;
            return;
        }
        grid.clearVisuals();
        solver1 = std::move(s1);
        solver2 = std::move(s2);
        isSolving = true;
        algorithmStartTime = std::chrono::steady_clock::now();
        panel.setAlgo1(n1, 0, 0, false, true);
        panel.setAlgo2(n2, 0, 0, false, true);
        std::cout << "\n>>> " << n1 << " vs " << n2 << " - RACE!" << std::endl;
    };

    auto startBoth = [&]() {
        startRace(std::make_unique<BFSSolver>(&grid),
                  std::make_unique<DFSSolver>(&grid),
                  "BFS", "DFS");
    };

    auto startAstarVsBfs = [&]() {
        startRace(std::make_unique<AStarSolver>(&grid),
                  std::make_unique<BFSSolver>(&grid),
                  "A*", "BFS");
    };

    auto printResults = [](Solver* s, long long ms) {
        std::cout << "  [" << s->getName() << "] Nodes: " << s->getNodesExplored()
                  << " | Path: " << s->getPathLength()
                  << " | Found: " << (s->hasPath() ? "YES" : "NO")
                  << " | Time: " << ms << "ms" << std::endl;
    };

    panel.setSpeed(animationSpeed);

    // ============================================
    // MAIN LOOP
    // ============================================
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }

            // ============================================
            // STARTUP SCREEN
            // ============================================
            if (appState == AppState::STARTUP) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Num1) {
                        gameMode = GameMode::CUSTOM;
                        appState = AppState::RUNNING;
                        panel.setGameMode("CUSTOM");
                        panel.setMode("WALL");
                        std::cout << "\n>>> CUSTOM mode" << std::endl;
                    }
                    if (event.key.code == sf::Keyboard::Num2) {
                        gameMode = GameMode::PRESET;
                        appState = AppState::RUNNING;
                        panel.setGameMode("PRESET");
                        loadPreset(Difficulty::BEGINNER, 1);
                        std::cout << "\n>>> PRESET mode" << std::endl;
                    }
                }
                continue;
            }

            // ============================================
            // MOUSE (Custom mode only for drawing)
            // ============================================
            if (gameMode == GameMode::CUSTOM) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.x >= MAZE_WIDTH) continue;
                    int row, col;
                    mouseToGrid(event.mouseButton.x, event.mouseButton.y, row, col);
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        if (grid.inBounds(row, col)) {
                            if (currentMode == PlaceMode::WALL) grid.toggleWall(row, col);
                            else applyMode(row, col);
                            isDragging = true;
                        }
                    }
                    if (event.mouseButton.button == sf::Mouse::Right) {
                        if (grid.inBounds(row, col)) grid.setStart(row, col);
                    }
                    if (event.mouseButton.button == sf::Mouse::Middle) {
                        if (grid.inBounds(row, col)) grid.setEnd(row, col);
                    }
                }
                if (event.type == sf::Event::MouseButtonReleased) {
                    if (event.mouseButton.button == sf::Mouse::Left) isDragging = false;
                }
            }

            if (event.type == sf::Event::MouseMoved) {
                int row, col;
                mouseToGrid(event.mouseMove.x, event.mouseMove.y, row, col);
                if (event.mouseMove.x < MAZE_WIDTH && grid.inBounds(row, col)) {
                    renderer.setHover(row, col);
                    if (isDragging && gameMode == GameMode::CUSTOM) {
                        if (currentMode == PlaceMode::WALL) grid.setWall(row, col);
                        else applyMode(row, col);
                    }
                } else {
                    renderer.clearHover();
                }
            }

            // ============================================
            // KEYBOARD
            // ============================================
            if (event.type == sf::Event::KeyPressed) {

                // Mode toggle (works in both modes)
                if (event.key.code == sf::Keyboard::M) {
                    if (gameMode == GameMode::CUSTOM) {
                        gameMode = GameMode::PRESET;
                        panel.setGameMode("PRESET");
                        loadPreset(Difficulty::BEGINNER, 1);
                    } else {
                        gameMode = GameMode::CUSTOM;
                        panel.setGameMode("CUSTOM");
                        panel.setPresetInfo("", 0, "");
                        grid.clearAll();
                        solver1 = nullptr; solver2 = nullptr; isSolving = false;
                        panel.clearAlgoStats();
                    }
                }

                // ============================================
                // CUSTOM MODE KEYS
                // ============================================
                if (gameMode == GameMode::CUSTOM) {
                    // Placement tools
                    if (event.key.code == sf::Keyboard::W) { currentMode = PlaceMode::WALL; panel.setMode("WALL"); }
                    if (event.key.code == sf::Keyboard::S) { currentMode = PlaceMode::START; panel.setMode("START"); }
                    if (event.key.code == sf::Keyboard::E) { currentMode = PlaceMode::END; panel.setMode("END"); }
                    if (event.key.code == sf::Keyboard::X) { currentMode = PlaceMode::ERASE; panel.setMode("ERASE"); }
                    // Terrain tools
                    if (event.key.code == sf::Keyboard::F) { currentMode = PlaceMode::SAND; panel.setMode("SAND"); }
                    if (event.key.code == sf::Keyboard::V) { currentMode = PlaceMode::WATER; panel.setMode("WATER"); }
                    if (event.key.code == sf::Keyboard::D) { currentMode = PlaceMode::MUD; panel.setMode("MUD"); }


                    // Algorithms (number keys)
                    if (event.key.code == sf::Keyboard::Num1) startBFS();
                    if (event.key.code == sf::Keyboard::Num2) startDFS();
                    if (event.key.code == sf::Keyboard::Num3) startBoth();
                    if (event.key.code == sf::Keyboard::Num4) startAStar();
                    if (event.key.code == sf::Keyboard::Num5) startDijkstra();
                    if (event.key.code == sf::Keyboard::Num6) startGreedy();
                    if (event.key.code == sf::Keyboard::Num7) startBidir();
                    if (event.key.code == sf::Keyboard::Num8) startIDDFS();
                    if (event.key.code == sf::Keyboard::Num9) startAstarVsBfs();
                }

                // ============================================
                // PRESET MODE KEYS
                // ============================================
                if (gameMode == GameMode::PRESET) {
                    // Difficulty selection
                    if (event.key.code == sf::Keyboard::B) { currentDifficulty = Difficulty::BEGINNER; loadPreset(currentDifficulty, currentLevel); }
                    if (event.key.code == sf::Keyboard::I) { currentDifficulty = Difficulty::INTERMEDIATE; loadPreset(currentDifficulty, currentLevel); }
                    if (event.key.code == sf::Keyboard::A) { currentDifficulty = Difficulty::ADVANCED; loadPreset(currentDifficulty, currentLevel); }

                    // Level loading (number keys 1-5)
                    if (event.key.code == sf::Keyboard::Num1) loadPreset(currentDifficulty, 1);
                    if (event.key.code == sf::Keyboard::Num2) loadPreset(currentDifficulty, 2);
                    if (event.key.code == sf::Keyboard::Num3) loadPreset(currentDifficulty, 3);
                    if (event.key.code == sf::Keyboard::Num4) loadPreset(currentDifficulty, 4);
                    if (event.key.code == sf::Keyboard::Num5) loadPreset(currentDifficulty, 5);

                    // Algorithms (letter keys)
                    if (event.key.code == sf::Keyboard::Q) startBFS();
                    if (event.key.code == sf::Keyboard::E) startDFS();
                    if (event.key.code == sf::Keyboard::R) startBoth();
                    if (event.key.code == sf::Keyboard::T) startAStar();
                    if (event.key.code == sf::Keyboard::Y) startDijkstra();
                    if (event.key.code == sf::Keyboard::U) startGreedy();
                    if (event.key.code == sf::Keyboard::O) startBidir();
                    if (event.key.code == sf::Keyboard::P) startIDDFS();
                    if (event.key.code == sf::Keyboard::Z) startAstarVsBfs();
                }

                // ============================================
                // COMMON KEYS (both modes)
                // ============================================

                // Speed
                if (event.key.code == sf::Keyboard::Add || event.key.code == sf::Keyboard::Equal) {
                    animationSpeed = std::max(1, animationSpeed - 5);
                    panel.setSpeed(animationSpeed);
                    std::cout << "Speed: " << animationSpeed << "ms" << std::endl;
                }
                if (event.key.code == sf::Keyboard::Subtract || event.key.code == sf::Keyboard::Hyphen) {
                    animationSpeed = std::min(200, animationSpeed + 5);
                    panel.setSpeed(animationSpeed);
                    std::cout << "Speed: " << animationSpeed << "ms" << std::endl;
                }

                // Commands
                if (event.key.code == sf::Keyboard::C) {
                    grid.clearAll();
                    solver1 = nullptr; solver2 = nullptr; isSolving = false;
                    panel.clearAlgoStats();
                    if (gameMode == GameMode::PRESET) panel.setPresetInfo("", 0, "");
                }
                if (event.key.code == sf::Keyboard::G) {
                    grid.clearVisuals();
                    solver1 = nullptr; solver2 = nullptr; isSolving = false;
                    panel.clearAlgoStats();
                }
            }
        }

        // ============================================
        // ALGORITHM STEP (Timed with turbo mode)
        // ============================================
        if (isSolving) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastStepTime).count();

            if (elapsed >= animationSpeed) {
                lastStepTime = now;

                // Turbo mode: run multiple steps per frame at high speed
                int stepsThisFrame = 1;
                if (animationSpeed <= 1)       stepsThisFrame = 20;
                else if (animationSpeed <= 3)  stepsThisFrame = 10;
                else if (animationSpeed <= 5)  stepsThisFrame = 5;
                else if (animationSpeed <= 10) stepsThisFrame = 3;
                else if (animationSpeed <= 20) stepsThisFrame = 2;

                bool anyActive = false;

                for (int s = 0; s < stepsThisFrame; s++) {
                    bool stepped = false;

                    if (solver1 && !solver1->isFinished()) {
                        solver1->step();
                        stepped = true;
                        anyActive = true;
                    }
                    if (solver2 && !solver2->isFinished()) {
                        solver2->step();
                        stepped = true;
                        anyActive = true;
                    }

                    if (!stepped) break;
                }

                // Update panel stats
                if (solver1) {
                    panel.setAlgo1(solver1->getName(), solver1->getNodesExplored(),
                                   solver1->getPathLength(), solver1->hasPath(), !solver1->isFinished());
                }
                if (solver2) {
                    panel.setAlgo2(solver2->getName(), solver2->getNodesExplored(),
                                   solver2->getPathLength(), solver2->hasPath(), !solver2->isFinished());
                }

                if (!anyActive) {
                    isSolving = false;
                    auto end = std::chrono::steady_clock::now();
                    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - algorithmStartTime).count();
                    std::cout << "\n--- ALGORITHM RESULTS ---" << std::endl;
                    if (solver1) printResults(solver1.get(), ms);
                    if (solver2) printResults(solver2.get(), ms);
                    std::cout << "-------------------------" << std::endl;
                }
            }
        }

        // ============================================
        // RENDER
        // ============================================
        window.clear(Colors::BG_DARK);
        if (appState == AppState::STARTUP) {
            static sf::Clock animClock;
            startupScreen.update(animClock.restart().asSeconds());
            startupScreen.render(window);
        } else {
            renderer.render(window, grid);
            panel.render(window);
        }
        window.display();
    }

    return 0;
}