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
    int pathCost;
    bool foundPath;
    bool finished;

    Algorithm algoType;

public:
    Solver(Grid* g, Algorithm type)
        : grid(g),
          nodesExplored(0),
          pathLength(0),
          pathCost(0),
          foundPath(false),
          finished(false),
          algoType(type)
    {
        int rows = grid->getRows();
        int cols = grid->getCols();

        parent.assign(rows, std::vector<std::pair<int,int>>(cols, {-1, -1}));
        visited.assign(rows, std::vector<bool>(cols, false));
    }

    virtual ~Solver() noexcept = default;

    virtual bool step() = 0;
    virtual std::string getName() const = 0;

    bool isFinished() const { return finished; }
    bool hasPath() const { return foundPath; }
    int getNodesExplored() const { return nodesExplored; }
    int getPathLength() const { return pathLength; }
    int getPathCost() const { return pathCost; }

    void markVisited(Cell& cell) {
        if (cell.isStart() || cell.isEnd()) return;

        switch (algoType) {
            case Algorithm::BFS: cell.bfsVisited = true; break;
            case Algorithm::DFS: cell.dfsVisited = true; break;
            case Algorithm::ASTAR: cell.astarVisited = true; break;
            case Algorithm::DIJKSTRA: cell.dijkstraVisited = true; break;
            case Algorithm::GREEDY: cell.greedyVisited = true; break;
            case Algorithm::BIDIRECTIONAL_BFS: cell.bidirVisited = true; break;
            case Algorithm::IDDFS: cell.iddfsVisited = true; break;
            default: break;
        }
    }

    void markPath(Cell& cell) {
        if (cell.isStart() || cell.isEnd()) return;

        switch (algoType) {
            case Algorithm::BFS: cell.bfsPath = true; break;
            case Algorithm::DFS: cell.dfsPath = true; break;
            case Algorithm::ASTAR: cell.astarPath = true; break;
            case Algorithm::DIJKSTRA: cell.dijkstraPath = true; break;
            case Algorithm::GREEDY: cell.greedyPath = true; break;
            case Algorithm::BIDIRECTIONAL_BFS: cell.bidirPath = true; break;
            case Algorithm::IDDFS: cell.iddfsPath = true; break;
            case Algorithm::QLEARNING: cell.qlearnPath = true; break;
            default: break;
        }
    }

    void unmarkPath(Cell& cell) {
        switch (algoType) {
            case Algorithm::BFS: cell.bfsPath = false; break;
            case Algorithm::DFS: cell.dfsPath = false; break;
            case Algorithm::ASTAR: cell.astarPath = false; break;
            case Algorithm::DIJKSTRA: cell.dijkstraPath = false; break;
            case Algorithm::GREEDY: cell.greedyPath = false; break;
            case Algorithm::BIDIRECTIONAL_BFS: cell.bidirPath = false; break;
            case Algorithm::IDDFS: cell.iddfsPath = false; break;
            case Algorithm::QLEARNING: cell.qlearnPath = false; break;
            default: break;
        }
    }

    void reconstructPath() {
        if (!foundPath) return;

        int endR = grid->getEndRow();
        int endC = grid->getEndCol();
        int startR = grid->getStartRow();
        int startC = grid->getStartCol();

        if (endR == -1 || endC == -1 || startR == -1 || startC == -1) {
            foundPath = false;
            pathLength = 0;
            pathCost = 0;
            return;
        }

        int r = endR;
        int c = endC;

        std::vector<std::pair<int,int>> path;

        while (r != -1 && c != -1) {
            path.push_back({r, c});

            if (r == startR && c == startC) break;

            auto p = parent[r][c];
            r = p.first;
            c = p.second;
        }

        if (path.empty() ||
            !(path.back().first == startR && path.back().second == startC)) {

            for (auto& [pr, pc] : path) {
                unmarkPath(grid->getCell(pr, pc));
            }

            foundPath = false;
            pathLength = 0;
            pathCost = 0;
            return;
        }

        pathLength = static_cast<int>(path.size());
        pathCost = 0;

        for (auto& [pr, pc] : path) {
            if (!(pr == startR && pc == startC)) {
                pathCost += grid->getCell(pr, pc).cost;
            }

            markPath(grid->getCell(pr, pc));
        }
    }
};
