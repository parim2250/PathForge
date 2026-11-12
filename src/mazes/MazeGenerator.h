#pragma once

#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include <utility>

#include "../core/Grid.h"

class MazeGenerator {
private:
    static std::mt19937& rng() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }

    // -------------------------------------------------------------------
    // 1. RECURSIVE DIVISION (Divide & Conquer)
    // Starts with an OPEN grid. Repeatedly adds straight walls dividing 
    // chambers into smaller rooms with single doorway gaps.
    // -------------------------------------------------------------------
    static void divideChamber(Grid& grid, int r1, int r2, int c1, int c2) {
        if (r2 - r1 < 2 || c2 - c1 < 2) return;

        bool horizontal = (r2 - r1) > (c2 - c1);
        if (r2 - r1 == c2 - c1) {
            std::uniform_int_distribution<int> coin(0, 1);
            horizontal = coin(rng()) == 1;
        }

        if (horizontal) {
            // Pick an even row for the wall
            std::uniform_int_distribution<int> rDist(r1 + 1, r2 - 1);
            int wallR = rDist(rng());
            if (wallR % 2 == 1) wallR++;

            if (wallR >= r2) wallR = r2 - 1;
            if (wallR <= r1) wallR = r1 + 1;

            // Draw horizontal wall across chamber
            for (int c = c1; c <= c2; ++c) grid.setWall(wallR, c);

            // Cut a doorway at an odd column
            std::uniform_int_distribution<int> doorDist(c1, c2);
            int doorC = doorDist(rng());
            if (doorC % 2 == 0) doorC++;
            if (doorC > c2) doorC = c2;
            if (doorC < c1) doorC = c1;

            grid.setEmpty(wallR, doorC);

            // Divide upper and lower sub-chambers
            divideChamber(grid, r1, wallR - 1, c1, c2);
            divideChamber(grid, wallR + 1, r2, c1, c2);
        } else {
            // Pick an even column for the wall
            std::uniform_int_distribution<int> cDist(c1 + 1, c2 - 1);
            int wallC = cDist(rng());
            if (wallC % 2 == 1) wallC++;

            if (wallC >= c2) wallC = c2 - 1;
            if (wallC <= c1) wallC = c1 + 1;

            // Draw vertical wall across chamber
            for (int r = r1; r <= r2; ++r) grid.setWall(r, wallC);

            // Cut a doorway at an odd row
            std::uniform_int_distribution<int> doorDist(r1, r2);
            int doorR = doorDist(rng());
            if (doorR % 2 == 0) doorR++;
            if (doorR > r2) doorR = r2;
            if (doorR < r1) doorR = r1;

            grid.setEmpty(doorR, wallC);

            // Divide left and right sub-chambers
            divideChamber(grid, r1, r2, c1, wallC - 1);
            divideChamber(grid, r1, r2, wallC + 1, c2);
        }
    }

    static void generateDivision(Grid& grid) {
        const int rows = grid.getRows();
        const int cols = grid.getCols();

        grid.clearAll(); // Start 100% empty

        // Draw outer border walls
        for (int r = 0; r < rows; ++r) { grid.setWall(r, 0); grid.setWall(r, cols - 1); }
        for (int c = 0; c < cols; ++c) { grid.setWall(0, c); grid.setWall(rows - 1, c); }

        // Divide the interior space
        divideChamber(grid, 1, rows - 2, 1, cols - 2);
    }

    // -------------------------------------------------------------------
    // 2. RECURSIVE BACKTRACKER (Depth-First Search / Stack)
    // Starts with 100% WALLS. Carves long, continuous, winding corridors.
    // -------------------------------------------------------------------
    static void generateBacktracker(Grid& grid) {
        const int rows = grid.getRows();
        const int cols = grid.getCols();

        grid.fillWalls(); // Start 100% walls

        std::vector<std::pair<int, int>> stack;
        stack.push_back({1, 1});
        grid.setEmpty(1, 1);

        const int dr[4] = {-2, 2, 0, 0};
        const int dc[4] = {0, 0, -2, 2};
        int lastDir = -1;

        while (!stack.empty()) {
            int r = stack.back().first;
            int c = stack.back().second;

            std::vector<int> candidates;
            for (int i = 0; i < 4; ++i) {
                int nr = r + dr[i];
                int nc = c + dc[i];
                if (nr > 0 && nr < rows - 1 && nc > 0 && nc < cols - 1) {
                    if (grid.isWall(nr, nc)) candidates.push_back(i);
                }
            }

            if (!candidates.empty()) {
                int chosenDir = -1;
                // Directional inertia: 75% chance to keep moving straight (creates long winding snakes)
                if (lastDir != -1) {
                    auto it = std::find(candidates.begin(), candidates.end(), lastDir);
                    std::uniform_real_distribution<float> inertia(0.0f, 1.0f);
                    if (it != candidates.end() && inertia(rng()) < 0.75f) {
                        chosenDir = lastDir;
                    }
                }

                if (chosenDir == -1) {
                    std::uniform_int_distribution<int> pick(0, (int)candidates.size() - 1);
                    chosenDir = candidates[pick(rng())];
                }

                lastDir = chosenDir;
                int nr = r + dr[chosenDir];
                int nc = c + dc[chosenDir];

                grid.setEmpty(nr, nc);
                grid.setEmpty((r + nr) / 2, (c + nc) / 2);
                stack.push_back({nr, nc});
            } else {
                stack.pop_back();
                lastDir = -1;
            }
        }
    }

    // -------------------------------------------------------------------
    // 3. PRIM'S ALGORITHM (Randomized Minimum Spanning Tree / Frontier)
    // Starts with 100% WALLS. Randomly picks frontier cells anywhere on 
    // the grid, creating a bushy, starburst maze with short dead ends.
    // -------------------------------------------------------------------
    static void generatePrims(Grid& grid) {
        const int rows = grid.getRows();
        const int cols = grid.getCols();

        grid.fillWalls(); // Start 100% walls
        grid.setEmpty(1, 1);

        const int dr[4] = {-2, 2, 0, 0};
        const int dc[4] = {0, 0, -2, 2};

        std::vector<std::pair<int, int>> frontier;

        auto addFrontier = [&](int r, int c) {
            for (int i = 0; i < 4; ++i) {
                int nr = r + dr[i];
                int nc = c + dc[i];
                if (nr > 0 && nr < rows - 1 && nc > 0 && nc < cols - 1) {
                    if (grid.isWall(nr, nc)) frontier.push_back({nr, nc});
                }
            }
        };

        addFrontier(1, 1);

        while (!frontier.empty()) {
            std::uniform_int_distribution<int> pick(0, (int)frontier.size() - 1);
            int idx = pick(rng());
            int r = frontier[idx].first;
            int c = frontier[idx].second;

            frontier[idx] = frontier.back();
            frontier.pop_back();

            if (!grid.isWall(r, c)) continue;

            std::vector<int> inMaze;
            for (int i = 0; i < 4; ++i) {
                int nr = r + dr[i];
                int nc = c + dc[i];
                if (nr > 0 && nr < rows - 1 && nc > 0 && nc < cols - 1) {
                    if (!grid.isWall(nr, nc)) inMaze.push_back(i);
                }
            }

            if (!inMaze.empty()) {
                std::uniform_int_distribution<int> pickLink(0, (int)inMaze.size() - 1);
                int dir = inMaze[pickLink(rng())];
                int nr = r + dr[dir];
                int nc = c + dc[dir];

                grid.setEmpty(r, c);
                grid.setEmpty((r + nr) / 2, (c + nc) / 2);
                addFrontier(r, c);
            }
        }
    }

public:
    static void generate(Grid& grid, const std::string& algo) {
        grid.clearAll();

        if (algo == "Backtracker") {
            generateBacktracker(grid);
        } else if (algo == "Prim's") {
            generatePrims(grid);
        } else if (algo == "Division") {
            generateDivision(grid);
        } else {
            generateBacktracker(grid);
        }

        // Set Start at top-left and End at bottom-right
        const int rows = grid.getRows();
        const int cols = grid.getCols();
        grid.setStart(1, 1);
        grid.setEnd(rows - 2, cols - 2);
    }
};