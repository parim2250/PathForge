#pragma once
#include "Solver.h"
#include <queue>
#include<vector>
#include <cmath>
#include <functional>

// ============================================
// GREEDY BEST-FIRST SEARCH
// Only uses heuristic (h) - ignores actual cost (g)
// Very fast but often NOT shortest path
// Beelines toward the goal, gets stuck at walls
// ============A================================

class GreedySolver : public Solver {
private:
    struct Node {
        int row, col;
        int hCost;

        bool operator>(const Node& other) const {
            return hCost > other.hCost;
        }
    };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> frontier;

    int endR, endC;

    int heuristic(int r, int c) const {
        return std::abs(r - endR) + std::abs(c - endC);
    }

public:
    GreedySolver(Grid* g):Solver(g, Algorithm::GREEDY) {
        int sr = grid->getStartRow();
        int sc = grid->getStartCol();
        endR = grid->getEndRow();
        endC = grid->getEndCol();

        if (sr != -1 && sc != -1 && endR != -1 && endC != -1) {
            visited[sr][sc] = true;
            frontier.push({sr, sc, heuristic(sr, sc)});
        } else {
            finished = true;
        }
    }

    std::string getName() const override { return "Greedy"; }

    bool step() override {
        if (finished) return false;
        if (frontier.empty()) {
            finished = true;
            return false;
        }

        Node current = frontier.top();
        frontier.pop();

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

            visited[nr][nc] = true;
            parent[nr][nc] = {current.row, current.col};
            frontier.push({nr, nc, heuristic(nr, nc)});
        }

        return true;
    }
};