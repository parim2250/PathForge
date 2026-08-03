#pragma once

// ============================================
// WINDOW CONFIGURATION
// ============================================
const int WINDOW_WIDTH    = 1400;
const int WINDOW_HEIGHT   = 800;

// ============================================
// UI PANEL (right side — used in later parts)
// ============================================
const int UI_PANEL_WIDTH  = 300;

// ============================================
// MAZE AREA
// ============================================
const int MAZE_WIDTH      = WINDOW_WIDTH - UI_PANEL_WIDTH;
const int MAZE_HEIGHT     = WINDOW_HEIGHT;

// ============================================
// GRID DIMENSIONS (must be odd for maze gen)
// ============================================
const int ROWS            = 31;
const int COLS            = 51;
const int CELL_SIZE       = MAZE_WIDTH / COLS;   // = 21 pixels per cell

// ============================================
// ANIMATION
// ============================================
const int DEFAULT_SPEED   = 30;   // milliseconds per step
const int MIN_SPEED       = 1;
const int MAX_SPEED       = 200;

// ============================================
// CELL TYPES (what a cell contains)
// ============================================
enum class Terrain {
    EMPTY,
    WALL,
    SAND,      // cost 3 (used in Part 4)
    WATER,     // cost 5 (used in Part 4)
    MUD,       // cost 8 (used in Part 4)
    START,
    END
};

// ============================================
// VISUALIZATION STATE (used by algorithms later)
// ============================================
enum class VisualState {
    NONE,           // not touched by any algorithm
    VISITED_BFS,
    VISITED_DFS,
    VISITED_ASTAR,
    VISITED_DIJKSTRA,
    VISITED_GREEDY,
    VISITED_BIDIR,
    FRONTIER,
    PATH_BFS,
    PATH_DFS,
    PATH_ASTAR,
    PATH_DIJKSTRA,
    PATH_GREEDY,
    PATH_BIDIR,
    PATH_QLEARN
};

// ============================================
// ALGORITHM IDS
// ============================================
enum class Algorithm {
    BFS,
    DFS,
    ASTAR,
    DIJKSTRA,
    GREEDY,
    BIDIRECTIONAL_BFS,
    IDDFS,
    QLEARNING
};

// ============================================
// MAZE GENERATOR TYPES
// ============================================
enum class GeneratorType {
    RECURSIVE_BACKTRACKER,
    PRIMS,
    KRUSKALS,
    RECURSIVE_DIVISION,
    WILSONS,
    RANDOM
};

// ============================================
// SOLVE STATE
// ============================================
enum class SolveState {
    IDLE,
    SOLVING,
    SOLVED,
    NO_PATH
};