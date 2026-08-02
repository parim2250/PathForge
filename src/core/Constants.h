#pragma once

const int WINDOW_WIDTH    = 1400;
const int WINDOW_HEIGHT   = 800;

const int UI_PANEL_WIDTH  = 300;

const int MAZE_WIDTH      = WINDOW_WIDTH - UI_PANEL_WIDTH;
const int MAZE_HEIGHT     = WINDOW_HEIGHT;

const int ROWS            = 31;   // always odd for maze generation
const int COLS            = 51;   // always odd for maze generation
const int CELL_SIZE       = MAZE_WIDTH / COLS;

const int DEFAULT_SPEED   = 30;
const int MIN_SPEED       = 1;
const int MAX_SPEED       = 200;

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

enum class Terrain {
    EMPTY,
    WALL,
    SAND,      // cost 3
    WATER,     // cost 5
    MUD,       // cost 8
    START,
    END
};

enum class GeneratorType {
    RECURSIVE_BACKTRACKER,
    PRIMS,
    KRUSKALS,
    RECURSIVE_DIVISION,
    WILSONS,
    RANDOM
};

enum class SolveState {
    IDLE,
    SOLVING,
    SOLVED,
    NO_PATH
};