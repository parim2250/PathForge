#pragma once
#include "Solver.h"
#include <queue>

class BFSSolver : public Solver {
private:
    std::queue<std::pair<int,int>> frontier;

public:
    BFSSolver(Grid* g) : Solver(g, Algorithm::BFS) {
        int sr = grid->getStartRow();
        int sc = grid->getStartCol();
        if (sr != -1 && sc != -1) {
            frontier.push({sr, sc});
            visited[sr][sc] = true;
        } else {
            finished = true;
        }
    }

    std::string getName() const override { return "BFS"; }

    bool step() override {
        if (finished) return false;
        if (frontier.empty()) {
            finished = true;
            return false;
        }

        auto [r, c] = frontier.front();
        frontier.pop();

        if (grid->getCell(r, c).isEnd()) {
            foundPath = true;
            finished = true;
            reconstructPath();
            return false;
        }

        markVisited(grid->getCell(r, c));
        nodesExplored++;

        for (int i = 0; i < 4; i++) {
            int nr = r + DR[i];
            int nc = c + DC[i];
            if (!grid->inBounds(nr, nc)) continue;
            if (visited[nr][nc]) continue;
            if (!grid->getCell(nr, nc).isWalkable()) continue;

            visited[nr][nc] = true;
            parent[nr][nc] = {r, c};
            frontier.push({nr, nc});
        }
        return true;
    }
};