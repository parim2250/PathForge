#pragma once
#include "../core/Grid.h"
#include "../core/Constants.h"
#include <vector>
#include <utility>
#include <string>

class Solver {
protected:
    Grid* grid;
    std::vector<std::vector<std::pair<int,int>>> parent;
    std::vector<std::vector<bool>> visited;

    int nodesExplored;
    int pathLength;
    bool foundPath;
    bool finished;

    // Which algorithm are we? (used to set the right flags on Cell)
    Algorithm algoType;

public:
    Solver(Grid* g, Algorithm type)
        : grid(g),
          nodesExplored(0),
          pathLength(0),
          foundPath(false),
          finished(false),
          algoType(type)
    {
        int rows = grid->getRows();
        int cols = grid->getCols();
        parent.assign(rows, std::vector<std::pair<int,int>>(cols, {-1, -1}));
        visited.assign(rows, std::vector<bool>(cols, false));
    }

    virtual ~Solver() {}
    virtual bool step() = 0;
    virtual std::string getName() const = 0;

    bool isFinished() const { return finished; }
    bool hasPath() const { return foundPath; }
    int getNodesExplored() const { return nodesExplored; }
    int getPathLength() const { return pathLength; }

    // ============================================
    // Mark a cell as VISITED by this algorithm
    // ============================================
    void markVisited(Cell& cell) {
        if (cell.isStart() || cell.isEnd()) return;
        switch (algoType) {
            case Algorithm::BFS:      cell.bfsVisited = true; break;
            case Algorithm::DFS:      cell.dfsVisited = true; break;
            case Algorithm::ASTAR:    cell.astarVisited = true; break;
            case Algorithm::DIJKSTRA: cell.dijkstraVisited = true; break;
            case Algorithm::GREEDY:   cell.greedyVisited = true; break;
            case Algorithm::BIDIRECTIONAL_BFS: cell.bidirVisited = true; break;
            default: break;
        }
    }

    // ============================================
    // Mark a cell as PATH by this algorithm
    // ============================================
    void markPath(Cell& cell) {
        if (cell.isStart() || cell.isEnd()) return;
        switch (algoType) {
            case Algorithm::BFS:      cell.bfsPath = true; break;
            case Algorithm::DFS:      cell.dfsPath = true; break;
            case Algorithm::ASTAR:    cell.astarPath = true; break;
            case Algorithm::DIJKSTRA: cell.dijkstraPath = true; break;
            case Algorithm::GREEDY:   cell.greedyPath = true; break;
            case Algorithm::BIDIRECTIONAL_BFS: cell.bidirPath = true; break;
            case Algorithm::QLEARNING: cell.qlearnPath = true; break;
            default: break;
        }
    }

    void reconstructPath() {
        if (!foundPath) return;

        int endR = grid->getEndRow();
        int endC = grid->getEndCol();
        int startR = grid->getStartRow();
        int startC = grid->getStartCol();

        int r = endR, c = endC;
        std::vector<std::pair<int,int>> path;

        while (!(r == startR && c == startC)) {
            path.push_back({r, c});
            auto p = parent[r][c];
            if (p.first == -1) break;
            r = p.first;
            c = p.second;
        }
        path.push_back({startR, startC});

        pathLength = path.size();

        for (auto& [pr, pc] : path) {
            markPath(grid->getCell(pr, pc));
        }
    }
};