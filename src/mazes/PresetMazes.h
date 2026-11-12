#pragma once

#include "../core/Grid.h"
#include "../core/Constants.h"
#include <string>
#include <algorithm>

// 15 hand-designed presets. Every maze has a guaranteed corridor from
// START to END. Terrain is applied through Grid setters so cell costs stay valid.
class PresetMazes {
public:
    struct MazeInfo {
        std::string name;
        std::string description;
    };

    static MazeInfo getInfo(Difficulty diff, int level) {
        static const MazeInfo easy[5] = {
            {"First Steps",       "A mostly open grid with a single gentle detour."},
            {"Corner Turn",       "Two broad barriers introduce simple route planning."},
            {"Twin Gates",        "Three barriers with wide, alternating openings."},
            {"Garden Walk",       "A winding corridor with a few harmless side pockets."},
            {"Sandy Trail",       "Several turns and a small sand patch along the route."}
        };
        static const MazeInfo medium[5] = {
            {"Switchback",        "A sequence of alternating passages across the grid."},
            {"Split Passage",     "Narrower gates and short dead-end branches."},
            {"Waterway",          "A winding route with water regions to avoid when possible."},
            {"Desert Crossing",   "Multiple barriers and broad sand fields."},
            {"Muddy Junction",    "A longer route with mixed sand, water and mud costs."}
        };
        static const MazeInfo hard[5] = {
            {"Narrow Escape",     "Many alternating one-cell gates demand careful navigation."},
            {"The Labyrinth",     "Dense partitions, offset openings and dead-end pockets."},
            {"Flooded Fortress",  "A complex passage network with expensive water zones."},
            {"The Long Detour",   "Repeated switchbacks, tight turns and mixed terrain."},
            {"Pathfinder's Trial","The most demanding layout: dense barriers and all terrain types."}
        };

        int index = std::clamp(level - 1, 0, 4);
        switch (diff) {
            case Difficulty::BEGINNER:     return easy[index];
            case Difficulty::INTERMEDIATE: return medium[index];
            case Difficulty::ADVANCED:     return hard[index];
        }
        return easy[0];
    }

    static void loadInto(Grid& grid, Difficulty diff, int level) {
        grid.clearAll();
        int safeLevel = std::clamp(level, 1, MAZES_PER_DIFFICULTY);

        switch (diff) {
            case Difficulty::BEGINNER:     loadEasy(grid, safeLevel); break;
            case Difficulty::INTERMEDIATE: loadMedium(grid, safeLevel); break;
            case Difficulty::ADVANCED:     loadHard(grid, safeLevel); break;
        }
    }

private:
    static void drawHWall(Grid& grid, int row, int c1, int c2) {
        if (row < 1 || row >= ROWS - 1) return;
        for (int c = std::max(1, c1); c <= std::min(COLS - 2, c2); ++c)
            grid.setWall(row, c);
    }

    static void drawVWall(Grid& grid, int col, int r1, int r2) {
        if (col < 1 || col >= COLS - 1) return;
        for (int r = std::max(1, r1); r <= std::min(ROWS - 2, r2); ++r)
            grid.setWall(r, col);
    }

    static void horizontalGate(Grid& grid, int row, int gapCol, int gapWidth = 2) {
        drawHWall(grid, row, 1, COLS - 2);
        int left = gapCol - (gapWidth - 1) / 2;
        for (int c = left; c < left + gapWidth; ++c)
            grid.setEmpty(row, c);
    }

    static void addPocketWalls(Grid& grid, int rowStart, int rowEnd,
                               int colStart, int colEnd, bool vertical) {
        if (vertical) {
            int c = (colStart + colEnd) / 2;
            drawVWall(grid, c, rowStart, rowEnd);
            grid.setEmpty((rowStart + rowEnd) / 2, c);
        } else {
            int r = (rowStart + rowEnd) / 2;
            drawHWall(grid, r, colStart, colEnd);
            grid.setEmpty(r, (colStart + colEnd) / 2);
        }
    }

    static void addTerrainPatch(Grid& grid, Terrain terrain,
                                int r1, int c1, int r2, int c2) {
        for (int r = std::max(1, r1); r <= std::min(ROWS - 2, r2); ++r) {
            for (int c = std::max(1, c1); c <= std::min(COLS - 2, c2); ++c) {
                if (!grid.getCell(r, c).isWall()) {
                    if (terrain == Terrain::SAND) grid.setSand(r, c);
                    else if (terrain == Terrain::WATER) grid.setWater(r, c);
                    else if (terrain == Terrain::MUD) grid.setMud(r, c);
                }
            }
        }
    }

    static void setEndpoints(Grid& grid) {
        grid.setStart(1, 1);
        grid.setEnd(ROWS - 2, COLS - 2);
    }

    // EASY: wide openings and a small number of barriers.
    static void loadEasy(Grid& grid, int level) {
        switch (level) {
            case 1:
                horizontalGate(grid, 15, 25, 10);
                break;

            case 2:
                horizontalGate(grid, 10, 39, 8);
                horizontalGate(grid, 21, 12, 8);
                break;

            case 3:
                horizontalGate(grid, 7, 12, 6);
                horizontalGate(grid, 15, 38, 6);
                horizontalGate(grid, 23, 14, 6);
                break;

            case 4:
                horizontalGate(grid, 6, 39, 5);
                horizontalGate(grid, 12, 12, 5);
                horizontalGate(grid, 18, 39, 5);
                horizontalGate(grid, 24, 12, 5);
                addPocketWalls(grid, 2, 5, 18, 30, true);
                addPocketWalls(grid, 20, 23, 22, 34, true);
                break;

            case 5:
            default:
                horizontalGate(grid, 5, 12, 4);
                horizontalGate(grid, 10, 39, 4);
                horizontalGate(grid, 15, 12, 4);
                horizontalGate(grid, 20, 39, 4);
                horizontalGate(grid, 25, 12, 4);
                addTerrainPatch(grid, Terrain::SAND, 2, 28, 7, 38);
                break;
        }
        setEndpoints(grid);
    }

    // MEDIUM: more barriers, narrower gates and terrain that rewards weighted search.
    static void loadMedium(Grid& grid, int level) {
        switch (level) {
            case 1:
                horizontalGate(grid, 5, 10, 3);
                horizontalGate(grid, 10, 40, 3);
                horizontalGate(grid, 15, 10, 3);
                horizontalGate(grid, 20, 40, 3);
                horizontalGate(grid, 25, 10, 3);
                break;

            case 2:
                horizontalGate(grid, 4, 38, 3);
                horizontalGate(grid, 9, 12, 3);
                horizontalGate(grid, 14, 38, 3);
                horizontalGate(grid, 19, 12, 3);
                horizontalGate(grid, 24, 38, 3);
                addPocketWalls(grid, 1, 3, 17, 30, true);
                addPocketWalls(grid, 16, 18, 20, 34, true);
                break;

            case 3:
                horizontalGate(grid, 4, 10, 2);
                horizontalGate(grid, 9, 40, 2);
                horizontalGate(grid, 14, 10, 2);
                horizontalGate(grid, 19, 40, 2);
                horizontalGate(grid, 24, 10, 2);
                addTerrainPatch(grid, Terrain::WATER, 2, 22, 7, 34);
                addTerrainPatch(grid, Terrain::WATER, 16, 5, 21, 18);
                break;

            case 4:
                horizontalGate(grid, 4, 39, 2);
                horizontalGate(grid, 9, 11, 2);
                horizontalGate(grid, 14, 39, 2);
                horizontalGate(grid, 19, 11, 2);
                horizontalGate(grid, 24, 39, 2);
                addTerrainPatch(grid, Terrain::SAND, 2, 18, 8, 33);
                addTerrainPatch(grid, Terrain::SAND, 16, 6, 22, 22);
                break;

            case 5:
            default:
                horizontalGate(grid, 4, 10, 2);
                horizontalGate(grid, 9, 40, 2);
                horizontalGate(grid, 14, 10, 2);
                horizontalGate(grid, 19, 40, 2);
                horizontalGate(grid, 24, 10, 2);
                addPocketWalls(grid, 1, 3, 17, 31, true);
                addPocketWalls(grid, 11, 13, 20, 35, true);
                addPocketWalls(grid, 21, 23, 17, 32, true);
                addTerrainPatch(grid, Terrain::SAND, 2, 22, 6, 34);
                addTerrainPatch(grid, Terrain::WATER, 11, 3, 16, 8);
                addTerrainPatch(grid, Terrain::MUD, 20, 25, 27, 36);
                break;
        }
        setEndpoints(grid);
    }

    // HARD: dense, narrow switchbacks. Each full-width barrier retains a gate,
    // so a valid route is guaranteed even as side pockets increase complexity.
    static void loadHard(Grid& grid, int level) {
        switch (level) {
            case 1:
                for (int r = 4, i = 0; r <= 26; r += 4, ++i)
                    horizontalGate(grid, r, (i % 2 == 0) ? 8 : 43, 1);
                break;

            case 2:
                for (int r = 3, i = 0; r <= 27; r += 4, ++i)
                    horizontalGate(grid, r, (i % 2 == 0) ? 42 : 9, 1);
                addPocketWalls(grid, 1, 2, 15, 34, true);
                addPocketWalls(grid, 9, 10, 16, 35, true);
                addPocketWalls(grid, 17, 18, 15, 34, true);
                addPocketWalls(grid, 25, 26, 16, 35, true);
                break;

            case 3:
                for (int r = 3, i = 0; r <= 27; r += 4, ++i)
                    horizontalGate(grid, r, (i % 2 == 0) ? 10 : 41, 1);
                addTerrainPatch(grid, Terrain::WATER, 1, 18, 8, 32);
                addTerrainPatch(grid, Terrain::MUD, 11, 3, 16, 14);
                addTerrainPatch(grid, Terrain::WATER, 19, 20, 27, 35);
                break;

            case 4:
                for (int r = 3, i = 0; r <= 27; r += 4, ++i)
                    horizontalGate(grid, r, (i % 2 == 0) ? 42 : 9, 1);
                addPocketWalls(grid, 1, 2, 14, 36, true);
                addPocketWalls(grid, 7, 8, 17, 33, true);
                addPocketWalls(grid, 15, 16, 14, 36, true);
                addPocketWalls(grid, 23, 24, 17, 33, true);
                addTerrainPatch(grid, Terrain::SAND, 1, 18, 6, 31);
                addTerrainPatch(grid, Terrain::MUD, 11, 18, 17, 31);
                break;

            case 5:
            default:
                for (int r = 3, i = 0; r <= 27; r += 4, ++i)
                    horizontalGate(grid, r, (i % 2 == 0) ? 9 : 42, 1);
                addPocketWalls(grid, 1, 2, 14, 36, true);
                addPocketWalls(grid, 5, 6, 17, 33, true);
                addPocketWalls(grid, 9, 10, 14, 36, true);
                addPocketWalls(grid, 13, 14, 17, 33, true);
                addPocketWalls(grid, 17, 18, 14, 36, true);
                addPocketWalls(grid, 21, 22, 17, 33, true);
                addPocketWalls(grid, 25, 26, 14, 36, true);
                addTerrainPatch(grid, Terrain::SAND, 1, 17, 6, 32);
                addTerrainPatch(grid, Terrain::WATER, 9, 18, 14, 34);
                addTerrainPatch(grid, Terrain::MUD, 17, 17, 23, 32);
                addTerrainPatch(grid, Terrain::WATER, 25, 20, 28, 38);
                break;
        }
        setEndpoints(grid);
    }
};
