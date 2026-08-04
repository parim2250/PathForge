#pragma once
#include "Solver.h"
#include <stack>

// ============================================
// ITERATIVE DEEPENING DFS (IDDFS)
// Runs Depth-Limited DFS repeatedly with growing depth
// Guaranteed to find shortest path (in unweighted graphs)
// Uses less memory than BFS
// ============================================

class IDDFSSolver : public Solver {
private:
    // Each stack entry stores position + depth
    struct DFSNode {
        int row, col, depth;
    };
    std::stack<DFSNode> frontier;

    int currentDepthLimit;
    int maxDepth;
    int sr, sc;

    // Track which cells are on current path (for depth limit enforcement)
    // Reset every iteration
    std::vector<std::vector<bool>> onCurrentIteration;

public:
    IDDFSSolver(Grid* g)
        : Solver(g, Algorithm::IDDFS),
          currentDepthLimit(1)
    {
        sr = grid->getStartRow();
        sc = grid->getStartCol();
        maxDepth = grid->getRows() * grid->getCols();

        int rows = grid->getRows();
        int cols = grid->getCols();
        onCurrentIteration.assign(rows, std::vector<bool>(cols, false));

        if (sr == -1 || sc == -1) {
            finished = true;
            return;
        }

        // Start first iteration at depth 1
        startNewIteration();
    }

    std::string getName() const override { return "IDDFS"; }

    bool step() override {
        if (finished) return false;

        // If stack is empty, current depth iteration finished without finding path
        // Start a new iteration with greater depth
        if (frontier.empty()) {
            if (currentDepthLimit >= maxDepth) {
                finished = true;
                return false;
            }
            currentDepthLimit += 2;
            startNewIteration();
            return true;
        }

        DFSNode current = frontier.top();
        frontier.pop();

        // Check if reached end
        if (grid->getCell(current.row, current.col).isEnd()) {
            foundPath = true;
            finished = true;
            reconstructPath();
            return false;
        }

        // Skip if already visited in this iteration
        if (onCurrentIteration[current.row][current.col]) {
            return true;
        }
        onCurrentIteration[current.row][current.col] = true;

        markVisited(grid->getCell(current.row, current.col));
        nodesExplored++;

        // Stop expanding at depth limit
        if (current.depth >= currentDepthLimit) return true;

        // Add all 4 neighbors
        for (int i = 0; i < 4; i++) {
            int nr = current.row + DR[i];
            int nc = current.col + DC[i];

            if (!grid->inBounds(nr, nc)) continue;
            if (!grid->getCell(nr, nc).isWalkable()) continue;
            if (onCurrentIteration[nr][nc]) continue;

            parent[nr][nc] = {current.row, current.col};
            frontier.push({nr, nc, current.depth + 1});
        }

        return true;
    }

private:
    void startNewIteration() {
        // Clear frontier
        while (!frontier.empty()) frontier.pop();

        // Reset iteration tracking (but keep visited visualization)
        int rows = grid->getRows();
        int cols = grid->getCols();
        onCurrentIteration.assign(rows, std::vector<bool>(cols, false));

        // Reset parent pointers so we get FRESH paths each iteration
        parent.assign(rows, std::vector<std::pair<int,int>>(cols, {-1, -1}));

        // Push start with depth 0
        frontier.push({sr, sc, 0});
    }
};