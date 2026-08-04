#pragma once
#include "Solver.h"
#include <queue>
#include <cmath>

// ============================================
// A* ALGORITHM
// Uses f(n) = g(n) + h(n)
//   g = actual cost from start
//   h = Manhattan distance to end (heuristic)
// Finds shortest path faster than BFS
// ============================================

class AStarSolver : public Solver {
private:
    // Node in the priority queue
    struct Node {
        int row, col;
        int gCost;     // steps from start
        int fCost;     // g + h

        // For priority queue: smaller f = higher priority
        bool operator>(const Node& other) const {
            return fCost > other.fCost;
        }
    };

    // Min-heap priority queue (smallest f on top)
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> frontier;

    // Track actual cost to reach each cell
    std::vector<std::vector<int>> gScore;

    int endR, endC;

    // Manhattan distance heuristic
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

        // Skip if we already found a better path
        if (visited[current.row][current.col]) return true;
        visited[current.row][current.col] = true;

        // Check if reached end
        if (grid->getCell(current.row, current.col).isEnd()) {
            foundPath = true;
            finished = true;
            reconstructPath();
            return false;
        }

        markVisited(grid->getCell(current.row, current.col));
        nodesExplored++;

        // Explore neighbors
        for (int i = 0; i < 4; i++) {
            int nr = current.row + DR[i];
            int nc = current.col + DC[i];

            if (!grid->inBounds(nr, nc)) continue;
            if (visited[nr][nc]) continue;
            if (!grid->getCell(nr, nc).isWalkable()) continue;

            int tentativeG = current.gCost + 1;

            if (tentativeG < gScore[nr][nc]) {
                gScore[nr][nc] = tentativeG;
                parent[nr][nc] = {current.row, current.col};
                int f = tentativeG + heuristic(nr, nc);
                frontier.push({nr, nc, tentativeG, f});
            }
        }

        return true;
    }
};