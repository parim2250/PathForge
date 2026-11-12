#pragma once
#include "Solver.h"
#include <stack>
#include <vector>
#include <climits>

class IDDFSSolver : public Solver {
private:
    struct DFSNode {
        int row, col, depth;
        int parentR, parentC;
    };
    std::stack<DFSNode> frontier;

    int currentDepthLimit;
    int maxDepth;
    int sr, sc, er, ec;

    std::vector<std::vector<int>> minDepthVisited;

public:
    IDDFSSolver(Grid* g)
        : Solver(g, Algorithm::IDDFS),
          currentDepthLimit(0)
    {
        sr = grid->getStartRow();
        sc = grid->getStartCol();
        er = grid->getEndRow();
        ec = grid->getEndCol();
        maxDepth = grid->getRows() * grid->getCols();

        int rows = grid->getRows();
        int cols = grid->getCols();
        minDepthVisited.assign(rows, std::vector<int>(cols, INT_MAX));

        if (sr == -1 || sc == -1 || er == -1 || ec == -1) {
            finished = true;
            return;
        }

        if (sr == er && sc == ec) {
            foundPath = true;
            finished = true;
            pathLength = 1;
            pathCost = 0;
            return;
        }

        startNewIteration();
    }

    std::string getName() const override { return "IDDFS"; }

    bool step() override {
        if (finished) return false;

        if (frontier.empty()) {
            if (currentDepthLimit >= maxDepth) {
                finished = true;
                return false;
            }
            currentDepthLimit++;
            startNewIteration();
            return true;
        }

        DFSNode current = frontier.top();
        frontier.pop();

        if (current.depth >= minDepthVisited[current.row][current.col]) {
            return true;
        }

        minDepthVisited[current.row][current.col] = current.depth;
        if (current.parentR != -1 && current.parentC != -1) {
            parent[current.row][current.col] = {current.parentR, current.parentC};
        }

        markVisited(grid->getCell(current.row, current.col));
        nodesExplored++;

        if (grid->getCell(current.row, current.col).isEnd()) {
            foundPath = true;
            finished = true;
            reconstructPath();
            return false;
        }

        if (current.depth >= currentDepthLimit) return true;

        for (int i = 0; i < 4; i++) {
            int nr = current.row + DR[i];
            int nc = current.col + DC[i];

            if (!grid->inBounds(nr, nc)) continue;
            if (!grid->getCell(nr, nc).isWalkable()) continue;

            if (current.depth + 1 < minDepthVisited[nr][nc]) {
                frontier.push({nr, nc, current.depth + 1, current.row, current.col});
            }
        }

        return true;
    }

private:
    void startNewIteration() {
        while (!frontier.empty()) frontier.pop();

        int rows = grid->getRows();
        int cols = grid->getCols();

        minDepthVisited.assign(rows, std::vector<int>(cols, INT_MAX));
        parent.assign(rows, std::vector<std::pair<int,int>>(cols, {-1, -1}));

        frontier.push({sr, sc, 0, -1, -1});
    }
};