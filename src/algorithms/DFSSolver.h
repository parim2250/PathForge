#pragma once
#include "Solver.h"
#include <stack>

class DFSSolver : public Solver {
private:
    std::stack<std::pair<int,int>> frontier;

public:
    DFSSolver(Grid* g) : Solver(g, Algorithm::DFS) {
        int sr = grid->getStartRow();
        int sc = grid->getStartCol();
        if (sr != -1 && sc != -1) {
            frontier.push({sr, sc});
            visited[sr][sc] = true;
        } else {
            finished = true;
        }
    }

    std::string getName() const override { return "DFS"; }

    bool step() override {
        if (finished) return false;
        if (frontier.empty()) {
            finished = true;
            return false;
        }

        auto [r, c] = frontier.top();
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