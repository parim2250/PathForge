#pragma once

// ============================================
// WINDOW & LAYOUT CONFIGURATION
// ============================================
const int WINDOW_WIDTH    = 1400;
const int WINDOW_HEIGHT   = 800;
const int UI_PANEL_WIDTH  = 340;  // Widened slightly for better button layout
const int MAZE_WIDTH      = WINDOW_WIDTH - UI_PANEL_WIDTH;
const int MAZE_HEIGHT     = WINDOW_HEIGHT;
const int TOP_NAV_HEIGHT  = 60;

// ============================================
// GRID DIMENSIONS (Centered with padding)
// ============================================
const int ROWS            = 31;
const int COLS            = 51;
const int CELL_SIZE       = 20; // 51*20 = 1020px width

// Auto-center the grid in the remaining space
const int GRID_OFFSET_X   = (MAZE_WIDTH - (COLS * CELL_SIZE)) / 2;
const int GRID_OFFSET_Y   = TOP_NAV_HEIGHT + (MAZE_HEIGHT - TOP_NAV_HEIGHT - (ROWS * CELL_SIZE)) / 2;

// ============================================
// ANIMATION
// ============================================
const int DEFAULT_SPEED   = 30;
const int MIN_SPEED       = 1;
const int MAX_SPEED       = 200;

// ============================================
// ENUMS (UNCHANGED)
// ============================================
enum class Terrain { EMPTY, WALL, SAND, WATER, MUD, START, END };

enum class VisualState {
    NONE, VISITED_BFS, VISITED_DFS, VISITED_ASTAR, VISITED_DIJKSTRA,
    VISITED_GREEDY, VISITED_BIDIR, FRONTIER, PATH_BFS, PATH_DFS, PATH_ASTAR,
    PATH_DIJKSTRA, PATH_GREEDY, PATH_BIDIR, PATH_QLEARN, PATH_OVERLAP
};

enum class Algorithm { BFS, DFS, ASTAR, DIJKSTRA, GREEDY, BIDIRECTIONAL_BFS, IDDFS, QLEARNING };
enum class GeneratorType { RECURSIVE_BACKTRACKER, PRIMS, KRUSKALS, RECURSIVE_DIVISION, WILSONS, RANDOM };
enum class SolveState { IDLE, SOLVING, SOLVED, NO_PATH };
enum class GameMode { CUSTOM, PRESET };
enum class Difficulty { BEGINNER, INTERMEDIATE, ADVANCED };

const int MAZES_PER_DIFFICULTY = 5;

// ============================================
// TERRAIN COSTS & DIRECTIONS
// ============================================
const int COST_EMPTY = 1;
const int COST_SAND  = 3;
const int COST_WATER = 5;
const int COST_MUD   = 8;

const int DR[4] = {-1, 0, 1, 0};
const int DC[4] = {0, 1, 0, -1};