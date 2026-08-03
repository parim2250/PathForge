#pragma once
#include "../core/Grid.h"
#include "../core/Constants.h"
#include <string>
#include <vector>
#include <cstdlib>

// ============================================
// PROGRAMMATICALLY GENERATED PRESET MAZES
// Balanced difficulty progression
// ============================================

class PresetMazes {
public:
    struct MazeInfo {
        std::string name;
        std::string description;
    };

    static MazeInfo getInfo(Difficulty diff, int level) {
        static const MazeInfo beginnerInfo[5] = {
            {"Open Field",      "Wide open space - fastest path is a diagonal"},
            {"Center Block",    "One wall in the middle - go around"},
            {"L Barrier",       "An L-shaped wall - simple detour"},
            {"Two Doors",       "Two rooms with narrow doorways"},
            {"Simple Weave",    "Three horizontal walls to navigate"}
        };

        static const MazeInfo intermediateInfo[5] = {
            {"Serpentine",      "Zigzag walls force a winding path"},
            {"Four Chambers",   "Four rooms connected by narrow doors"},
            {"Cross Maze",      "Cross-shaped barriers create decisions"},
            {"Comb Structure",  "Comb-like walls with dead ends"},
            {"Dual Spiral",     "Twin spirals - watch algorithms explore"}
        };

        static const MazeInfo advancedInfo[5] = {
            {"Recursive Grid",  "Dense grid pattern - stress test BFS"},
            {"Nested Rooms",    "Rooms within rooms - complex traversal"},
            {"Broken Fortress", "Concentric walls with strategic gaps"},
            {"Web of Dead Ends","Many dead ends - watch DFS get confused"},
            {"The Gauntlet",    "Ultimate challenge - long winding path"}
        };

        switch (diff) {
            case Difficulty::BEGINNER:     return beginnerInfo[std::min(std::max(level - 1, 0), 4)];
            case Difficulty::INTERMEDIATE: return intermediateInfo[std::min(std::max(level - 1, 0), 4)];
            case Difficulty::ADVANCED:     return advancedInfo[std::min(std::max(level - 1, 0), 4)];
        }
        return beginnerInfo[0];
    }

    static void loadInto(Grid& grid, Difficulty diff, int level) {
        grid.clearAll();

        switch (diff) {
            case Difficulty::BEGINNER:     loadBeginner(grid, level); break;
            case Difficulty::INTERMEDIATE: loadIntermediate(grid, level); break;
            case Difficulty::ADVANCED:     loadAdvanced(grid, level); break;
        }
    }

private:
    // Helpers
    static void drawHWall(Grid& grid, int row, int colStart, int colEnd) {
        for (int c = colStart; c <= colEnd && c < COLS; c++)
            if (row >= 0 && row < ROWS) grid.setWall(row, c);
    }

    static void drawVWall(Grid& grid, int col, int rowStart, int rowEnd) {
        for (int r = rowStart; r <= rowEnd && r < ROWS; r++)
            if (col >= 0 && col < COLS) grid.setWall(r, col);
    }

    static void drawBorder(Grid& grid) {
        drawHWall(grid, 0, 0, COLS - 1);
        drawHWall(grid, ROWS - 1, 0, COLS - 1);
        drawVWall(grid, 0, 0, ROWS - 1);
        drawVWall(grid, COLS - 1, 0, ROWS - 1);
    }

    // ============================================
    // BEGINNER MAZES (Easy — Easy-Medium)
    // ============================================
    static void loadBeginner(Grid& grid, int level) {
        drawBorder(grid);

        switch (level) {
            case 1: {
                // Open Field - minimal obstacles
                grid.setStart(2, 2);
                grid.setEnd(ROWS - 3, COLS - 3);
                break;
            }
            case 2: {
                // Center Block - one wall
                int midR = ROWS / 2;
                int midC = COLS / 2;
                drawVWall(grid, midC, midR - 5, midR + 5);
                grid.setStart(midR, 3);
                grid.setEnd(midR, COLS - 4);
                break;
            }
            case 3: {
                // L Barrier
                drawHWall(grid, ROWS / 2, 5, COLS / 2);
                drawVWall(grid, COLS / 2, ROWS / 2, ROWS - 8);
                grid.setStart(2, 2);
                grid.setEnd(ROWS - 3, COLS - 3);
                break;
            }
            case 4: {
                // Two Doors - two rooms
                int midC = COLS / 2;
                drawVWall(grid, midC, 1, ROWS - 2);
                // Create two doors
                grid.getCell(ROWS / 3, midC).terrain = Terrain::EMPTY;
                grid.getCell(ROWS / 3 + 1, midC).terrain = Terrain::EMPTY;
                grid.getCell(2 * ROWS / 3, midC).terrain = Terrain::EMPTY;
                grid.getCell(2 * ROWS / 3 + 1, midC).terrain = Terrain::EMPTY;
                grid.setStart(2, 3);
                grid.setEnd(ROWS - 3, COLS - 4);
                break;
            }
            case 5:
            default: {
                // Simple Weave - three horizontal walls
                drawHWall(grid, ROWS / 4, 1, COLS - 8);
                drawHWall(grid, ROWS / 2, 7, COLS - 2);
                drawHWall(grid, 3 * ROWS / 4, 1, COLS - 8);
                grid.setStart(2, 2);
                grid.setEnd(ROWS - 3, COLS - 3);
                break;
            }
        }
    }

    // ============================================
    // INTERMEDIATE MAZES (Medium — Medium-Hard)
    // ============================================
    static void loadIntermediate(Grid& grid, int level) {
        drawBorder(grid);

        switch (level) {
            case 1: {
                // Serpentine - 5 alternating horizontal walls
                for (int i = 1; i <= 5; i++) {
                    int row = i * (ROWS / 6);
                    if (i % 2 == 1) drawHWall(grid, row, 1, COLS - 8);
                    else drawHWall(grid, row, 7, COLS - 2);
                }
                grid.setStart(1, 1);
                grid.setEnd(ROWS - 2, COLS - 2);
                break;
            }
            case 2: {
                // Four Chambers
                int midR = ROWS / 2;
                int midC = COLS / 2;
                drawHWall(grid, midR, 1, COLS - 2);
                drawVWall(grid, midC, 1, ROWS - 2);
                // Doors
                grid.getCell(midR, COLS / 4).terrain = Terrain::EMPTY;
                grid.getCell(midR, COLS / 4 + 1).terrain = Terrain::EMPTY;
                grid.getCell(midR, 3 * COLS / 4).terrain = Terrain::EMPTY;
                grid.getCell(midR, 3 * COLS / 4 + 1).terrain = Terrain::EMPTY;
                grid.getCell(ROWS / 4, midC).terrain = Terrain::EMPTY;
                grid.getCell(ROWS / 4 + 1, midC).terrain = Terrain::EMPTY;
                grid.getCell(3 * ROWS / 4, midC).terrain = Terrain::EMPTY;
                grid.getCell(3 * ROWS / 4 + 1, midC).terrain = Terrain::EMPTY;
                grid.setStart(2, 2);
                grid.setEnd(ROWS - 3, COLS - 3);
                break;
            }
            case 3: {
                // Cross Maze - + shaped walls
                int midR = ROWS / 2;
                int midC = COLS / 2;
                drawHWall(grid, midR, 3, midC - 3);
                drawHWall(grid, midR, midC + 3, COLS - 4);
                drawVWall(grid, midC, 3, midR - 3);
                drawVWall(grid, midC, midR + 3, ROWS - 4);
                // Second cross
                drawHWall(grid, ROWS / 4, midC / 2, midC + midC / 2);
                drawVWall(grid, COLS / 4, ROWS / 4 - 3, ROWS / 4 + 3);
                grid.setStart(2, 2);
                grid.setEnd(ROWS - 3, COLS - 3);
                break;
            }
            case 4: {
                // Comb Structure - many dead ends
                for (int c = 4; c < COLS - 4; c += 5) {
                    drawVWall(grid, c, 1, ROWS - 6);
                }
                for (int c = 6; c < COLS - 4; c += 5) {
                    drawVWall(grid, c, 5, ROWS - 2);
                }
                grid.setStart(1, 1);
                grid.setEnd(ROWS - 2, COLS - 2);
                break;
            }
            case 5:
            default: {
                // Dual Spiral - two spirals
                int q1CenterR = ROWS / 4;
                int q1CenterC = COLS / 4;
                int q2CenterR = 3 * ROWS / 4;
                int q2CenterC = 3 * COLS / 4;

                // First spiral (top-left)
                for (int i = 1; i <= 3; i++) {
                    int m = i * 2;
                    if (q1CenterR - m > 0 && q1CenterC - m > 0) {
                        drawHWall(grid, q1CenterR - m, q1CenterC - m, q1CenterC + m - 1);
                        drawVWall(grid, q1CenterC + m, q1CenterR - m, q1CenterR + m);
                        drawHWall(grid, q1CenterR + m, q1CenterC - m + 1, q1CenterC + m);
                        drawVWall(grid, q1CenterC - m, q1CenterR - m + 1, q1CenterR + m - 1);
                    }
                }

                // Second spiral (bottom-right)
                for (int i = 1; i <= 3; i++) {
                    int m = i * 2;
                    if (q2CenterR + m < ROWS - 1 && q2CenterC + m < COLS - 1) {
                        drawHWall(grid, q2CenterR - m, q2CenterC - m, q2CenterC + m - 1);
                        drawVWall(grid, q2CenterC + m, q2CenterR - m, q2CenterR + m);
                        drawHWall(grid, q2CenterR + m, q2CenterC - m + 1, q2CenterC + m);
                        drawVWall(grid, q2CenterC - m, q2CenterR - m + 1, q2CenterR + m - 1);
                    }
                }

                grid.setStart(1, 1);
                grid.setEnd(ROWS - 2, COLS - 2);
                break;
            }
        }
    }

    // ============================================
    // ADVANCED MAZES (Hard — Extreme)
    // ============================================
    static void loadAdvanced(Grid& grid, int level) {
        drawBorder(grid);

        switch (level) {
            case 1: {
                // Recursive Grid - dense grid pattern
                for (int r = 2; r < ROWS - 2; r += 2) {
                    for (int c = 2; c < COLS - 2; c += 2) {
                        grid.setWall(r, c);
                    }
                }
                // Add connections randomly (fixed seed)
                srand(42);
                for (int r = 2; r < ROWS - 2; r += 2) {
                    for (int c = 3; c < COLS - 3; c += 2) {
                        if (rand() % 3 == 0) grid.setWall(r, c);
                    }
                }
                for (int r = 3; r < ROWS - 3; r += 2) {
                    for (int c = 2; c < COLS - 2; c += 2) {
                        if (rand() % 3 == 0) grid.setWall(r, c);
                    }
                }
                grid.setStart(1, 1);
                grid.setEnd(ROWS - 2, COLS - 2);
                break;
            }
            case 2: {
                // Nested Rooms - rooms within rooms
                for (int i = 1; i <= 4; i++) {
                    int margin = i * 3;
                    if (margin >= ROWS / 2 || margin >= COLS / 2) break;

                    drawHWall(grid, margin, margin, COLS - margin - 1);
                    drawHWall(grid, ROWS - margin - 1, margin, COLS - margin - 1);
                    drawVWall(grid, margin, margin, ROWS - margin - 1);
                    drawVWall(grid, COLS - margin - 1, margin, ROWS - margin - 1);

                    // Door at different position per layer
                    int doorR, doorC;
                    if (i % 4 == 1) {
                        doorR = margin;
                        doorC = COLS - margin - 5;
                    } else if (i % 4 == 2) {
                        doorR = ROWS - margin - 1;
                        doorC = margin + 3;
                    } else if (i % 4 == 3) {
                        doorR = margin + 3;
                        doorC = margin;
                    } else {
                        doorR = ROWS - margin - 4;
                        doorC = COLS - margin - 1;
                    }
                    grid.getCell(doorR, doorC).terrain = Terrain::EMPTY;
                    if (doorR + 1 < ROWS - 1) grid.getCell(doorR + 1, doorC).terrain = Terrain::EMPTY;
                }
                grid.setStart(1, 1);
                grid.setEnd(ROWS / 2, COLS / 2);
                break;
            }
            case 3: {
                // Broken Fortress
                for (int i = 1; i <= 5; i++) {
                    int margin = i * 3;
                    if (margin >= ROWS / 2 || margin >= COLS / 2) break;

                    drawHWall(grid, margin, margin, COLS - margin - 1);
                    drawHWall(grid, ROWS - margin - 1, margin, COLS - margin - 1);
                    drawVWall(grid, margin, margin, ROWS - margin - 1);
                    drawVWall(grid, COLS - margin - 1, margin, ROWS - margin - 1);

                    // Two openings per layer (random positions)
                    srand(100 + i);
                    int side1 = rand() % 4;
                    int side2 = (side1 + 2) % 4;

                    auto makeOpening = [&](int side, int layer) {
                        int m = layer * 3;
                        switch (side) {
                            case 0: // top
                                grid.getCell(m, COLS / 2).terrain = Terrain::EMPTY;
                                grid.getCell(m, COLS / 2 + 1).terrain = Terrain::EMPTY;
                                break;
                            case 1: // right
                                grid.getCell(ROWS / 2, COLS - m - 1).terrain = Terrain::EMPTY;
                                grid.getCell(ROWS / 2 + 1, COLS - m - 1).terrain = Terrain::EMPTY;
                                break;
                            case 2: // bottom
                                grid.getCell(ROWS - m - 1, COLS / 2).terrain = Terrain::EMPTY;
                                grid.getCell(ROWS - m - 1, COLS / 2 + 1).terrain = Terrain::EMPTY;
                                break;
                            case 3: // left
                                grid.getCell(ROWS / 2, m).terrain = Terrain::EMPTY;
                                grid.getCell(ROWS / 2 + 1, m).terrain = Terrain::EMPTY;
                                break;
                        }
                    };
                    makeOpening(side1, i);
                    makeOpening(side2, i);
                }
                grid.setStart(1, 1);
                grid.setEnd(ROWS / 2, COLS / 2);
                break;
            }
            case 4: {
                // Web of Dead Ends
                srand(7);
                // Create many small vertical walls with gaps
                for (int i = 0; i < 20; i++) {
                    int startR = 2 + (rand() % (ROWS - 8));
                    int startC = 2 + (rand() % (COLS - 4));
                    int length = 3 + (rand() % 6);

                    if (rand() % 2 == 0) {
                        drawVWall(grid, startC, startR, std::min(startR + length, ROWS - 2));
                    } else {
                        drawHWall(grid, startR, startC, std::min(startC + length, COLS - 2));
                    }
                }
                grid.setStart(1, 1);
                grid.setEnd(ROWS - 2, COLS - 2);
                break;
            }
            case 5:
            default: {
                // The Gauntlet - very long winding path
                // Create alternating long walls
                for (int i = 1; i < 12; i++) {
                    int row = i * 2 + 1;
                    if (row >= ROWS - 2) break;
                    if (i % 2 == 1) {
                        drawHWall(grid, row, 1, COLS - 6);
                    } else {
                        drawHWall(grid, row, 5, COLS - 2);
                    }
                }
                grid.setStart(1, 1);
                grid.setEnd(ROWS - 2, COLS - 2);
                break;
            }
        }
    }
};