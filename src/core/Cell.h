#pragma once
#include "Constants.h"

class Cell {
public:
    // Position (never changes)
    int row;
    int col;

    // What kind of cell it is (wall, empty, start, end, terrain)
    Terrain terrain;

    // How the algorithm currently sees it (visited, in path, frontier)
    VisualState visualState;

    // Cost to enter this cell (used in weighted mazes — Part 4)
    int cost;

    // ============================================
    // CONSTRUCTOR
    // ============================================
    Cell(int r = 0, int c = 0)
        : row(r),
          col(c),
          terrain(Terrain::EMPTY),
          visualState(VisualState::NONE),
          cost(1)
    {}

    // ============================================
    // HELPER METHODS
    // ============================================

    // Can a solver walk through this cell?
    bool isWalkable() const {
        return terrain != Terrain::WALL;
    }

    // Is this the start point?
    bool isStart() const {
        return terrain == Terrain::START;
    }

    // Is this the end point?
    bool isEnd() const {
        return terrain == Terrain::END;
    }

    // Is this a wall?
    bool isWall() const {
        return terrain == Terrain::WALL;
    }

    // Reset only visualization (keep walls, start, end)
    void resetVisual() {
        visualState = VisualState::NONE;
    }

    // Complete reset (remove everything)
    void reset() {
        terrain = Terrain::EMPTY;
        visualState = VisualState::NONE;
        cost = 1;
    }
};