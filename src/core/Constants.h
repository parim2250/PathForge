#pragma once

// ============================================
// WINDOW CONFIGURATION
// ============================================
const int WINDOW_WIDTH    = 1400;
const int WINDOW_HEIGHT   = 800;

// ============================================
// UI PANEL
// ============================================
const int UI_PANEL_WIDTH  = 300;

// ============================================
// MAZE AREA
// ============================================
const int MAZE_WIDTH      = WINDOW_WIDTH - UI_PANEL_WIDTH;
const int MAZE_HEIGHT     = WINDOW_HEIGHT;

// ============================================
// GRID DIMENSIONS (odd for maze generation)
// ============================================
const int ROWS            = 31;
const int COLS            = 51;
const int CELL_SIZE       = MAZE_WIDTH / COLS;

// ============================================
// ANIMATION
// ============================================
const int DEFAULT_SPEED   = 30;
const int MIN_SPEED       = 1;
const int MAX_SPEED       = 200;

// ============================================
// CELL TYPES
// ============================================
enum class Terrain {
    EMPTY,
    WALL,
    SAND,
    WATER,
    MUD,
    START,
    END
};

// ============================================
// VISUALIZATION STATE
// ============================================
enum class VisualState {
    NONE,
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

// ============================================
// GAME MODE
// ============================================
enum class GameMode {
    CUSTOM,
    PRESET
};

// ============================================
// DIFFICULTY
// ============================================
enum class Difficulty {
    BEGINNER,
    INTERMEDIATE,
    ADVANCED
};

const int MAZES_PER_DIFFICULTY = 5;

// ============================================
// MOVEMENT DIRECTIONS (4-directional)
// UP, RIGHT, DOWN, LEFT
// ============================================
const int DR[4] = {-1, 0, 1, 0};
const int DC[4] = {0, 1, 0, -1};