#pragma once
#include "Constants.h"

class Cell {
public:
    int row;
    int col;
    Terrain terrain;

    // OLD single visualState (kept for compatibility)
    VisualState visualState;

    // Multi-algorithm tracking flags
    bool bfsVisited;
    bool dfsVisited;
    bool astarVisited;
    bool dijkstraVisited;
    bool greedyVisited;
    bool bidirVisited;
    bool iddfsVisited;  // NEW

    bool bfsPath;
    bool dfsPath;
    bool astarPath;
    bool dijkstraPath;
    bool greedyPath;
    bool bidirPath;
    bool iddfsPath;     // NEW
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
          iddfsVisited(false),  // NEW
          bfsPath(false), dfsPath(false),
          astarPath(false), dijkstraPath(false),
          greedyPath(false), bidirPath(false),
          iddfsPath(false),     // NEW
          qlearnPath(false),
          isFrontier(false),
          cost(COST_EMPTY)
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
        iddfsVisited = false;  // NEW

        bfsPath = dfsPath = astarPath = false;
        dijkstraPath = greedyPath = bidirPath = false;
        iddfsPath = false;     // NEW

        qlearnPath = false;
        isFrontier = false;
    }

    // Complete reset
    void reset() {
        terrain = Terrain::EMPTY;
        resetVisual();
        cost = COST_EMPTY;
    }
};