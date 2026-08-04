#pragma once
#include "Constants.h"

class Cell {
public:
    int row;
    int col;
    Terrain terrain;

    // ============================================
    // OLD single visualState (kept for compatibility)
    // ============================================
    VisualState visualState;

    // ============================================
    // NEW: Multi-algorithm tracking flags
    // These let us show overlaps beautifully
    // ============================================
    bool bfsVisited;
    bool dfsVisited;
    bool astarVisited;
    bool dijkstraVisited;
    bool greedyVisited;
    bool bidirVisited;

    bool bfsPath;
    bool dfsPath;
    bool astarPath;
    bool dijkstraPath;
    bool greedyPath;
    bool bidirPath;
    bool qlearnPath;

    bool isFrontier;
    int cost;

    Cell(int r = 0, int c = 0)
        : row(r), col(c),
          terrain(Terrain::EMPTY),
          visualState(VisualState::NONE),
          bfsVisited(false), dfsVisited(false),
          astarVisited(false), dijkstraVisited(false),
          greedyVisited(false), bidirVisited(false),
          bfsPath(false), dfsPath(false),
          astarPath(false), dijkstraPath(false),
          greedyPath(false), bidirPath(false),
          qlearnPath(false),
          isFrontier(false),
          cost(1)
    {}

    bool isWalkable() const { return terrain != Terrain::WALL; }
    bool isStart() const { return terrain == Terrain::START; }
    bool isEnd() const { return terrain == Terrain::END; }
    bool isWall() const { return terrain == Terrain::WALL; }

    // Reset only visualization flags (keep walls/start/end)
    void resetVisual() {
        visualState = VisualState::NONE;
        bfsVisited = dfsVisited = astarVisited = false;
        dijkstraVisited = greedyVisited = bidirVisited = false;
        bfsPath = dfsPath = astarPath = false;
        dijkstraPath = greedyPath = bidirPath = false;
        qlearnPath = false;
        isFrontier = false;
    }

    // Complete reset
    void reset() {
        terrain = Terrain::EMPTY;
        resetVisual();
        cost = 1;
    }
};