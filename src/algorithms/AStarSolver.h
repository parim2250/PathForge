#pragma once
#include "Solver.h"
#include <queue>
#include <cmath>
#include <climits>

class AStarSolver : public Solver {
private:
    struct Node {
        int row, col;
        int gCost;
        int fCost;
        bool operator>(const Node& other) const {
            if (fCost == other.fCost) return gCost > other.gCost;
            return fCost > other.fCost;
        }
    };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> frontier;
    std::vector<std::vector<int>> gScore;
    int endR, endC;

    int heuristic(int r, int c) const {
        return std::abs(r - endR) + std::abs(c - endC);
    }

public:
    AStarSolver(Grid* g) : Solver(g, Algorithm::ASTAR) {
        int rows = grid->getRows();
        int cols = grid->getCols();
        gScore.assign(rows, std::vector<int>(cols, INT_MAX));

        int sr = grid->getStartRow();
        int sc = grid->getStartCol();
        endR = grid->getEndRow();
        endC = grid->getEndCol();

        if (sr != -1 && sc != -1 && endR != -1 && endC != -1) {
            gScore[sr][sc] = 0;
            frontier.push({sr, sc, 0, heuristic(sr, sc)});
        } else {
            finished = true;
        }
    }

    std::string getName() const override { return "A*"; }

    bool step() override {
        if (finished) return false;
        if (frontier.empty()) {
            finished = true;
            return false;
        }

        Node current = frontier.top();
        frontier.pop();

        if (visited[current.row][current.col]) return true;
        visited[current.row][current.col] = true;

        if (grid->getCell(current.row, current.col).isEnd()) {
            foundPath = true;
            finished = true;
            reconstructPath();
            return false;
        }

        markVisited(grid->getCell(current.row, current.col));
        nodesExplored++;

        for (int i = 0; i < 4; i++) {
            int nr = current.row + DR[i];
            int nc = current.col + DC[i];

            if (!grid->inBounds(nr, nc)) continue;
            if (visited[nr][nc]) continue;
            if (!grid->getCell(nr, nc).isWalkable()) continue;

            // CORRECT WEIGHT HANDLING: Use the actual cost of the cell being entered
            int cellCost = grid->getCell(nr, nc).cost;
            int tentativeG = current.gCost + cellCost;

            if (tentativeG < gScore[nr][nc]) {
                gScore[nr][nc] = tentativeG;
                parent[nr][nc] = {current.row, current.col};
                frontier.push({nr, nc, tentativeG, tentativeG + heuristic(nr, nc)});
            }
        }
        return true;
    }
};