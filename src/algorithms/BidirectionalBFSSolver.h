#pragma once
#include "Solver.h"
#include <queue>

// ============================================
// BIDIRECTIONAL BFS
// Runs TWO BFS simultaneously:
//   Forward from Start
//   Backward from End
// They meet in the middle - much faster!
// ============================================

class BidirectionalBFSSolver : public Solver {
private:
    std::queue<std::pair<int,int>> frontierStart;
    std::queue<std::pair<int,int>> frontierEnd;

    // Separate visited/parent arrays for each direction
    std::vector<std::vector<bool>> visitedStart;
    std::vector<std::vector<bool>> visitedEnd;
    std::vector<std::vector<std::pair<int,int>>> parentStart;
    std::vector<std::vector<std::pair<int,int>>> parentEnd;

    // Meeting point when they collide
    int meetR = -1, meetC = -1;
    bool searchFromStart = true;  // alternate turns

public:
    BidirectionalBFSSolver(Grid* g) : Solver(g, Algorithm::BIDIRECTIONAL_BFS) {
        int rows = grid->getRows();
        int cols = grid->getCols();

        visitedStart.assign(rows, std::vector<bool>(cols, false));
        visitedEnd.assign(rows, std::vector<bool>(cols, false));
        parentStart.assign(rows, std::vector<std::pair<int,int>>(cols, {-1, -1}));
        parentEnd.assign(rows, std::vector<std::pair<int,int>>(cols, {-1, -1}));

        int sr = grid->getStartRow();
        int sc = grid->getStartCol();
        int er = grid->getEndRow();
        int ec = grid->getEndCol();

        if (sr != -1 && er != -1) {
            frontierStart.push({sr, sc});
            frontierEnd.push({er, ec});
            visitedStart[sr][sc] = true;
            visitedEnd[er][ec] = true;
        } else {
            finished = true;
        }
    }

    std::string getName() const override { return "BiDir BFS"; }

    bool step() override {
        if (finished) return false;
        if (frontierStart.empty() && frontierEnd.empty()) {
            finished = true;
            return false;
        }

        // Alternate between the two searches
        if (searchFromStart && !frontierStart.empty()) {
            if (expandOne(frontierStart, visitedStart, parentStart, visitedEnd)) {
                return false;
            }
        } else if (!frontierEnd.empty()) {
            if (expandOne(frontierEnd, visitedEnd, parentEnd, visitedStart)) {
                return false;
            }
        }
        searchFromStart = !searchFromStart;
        return true;
    }

private:
    // Expand one cell from a direction. Returns true if collision found.
    bool expandOne(std::queue<std::pair<int,int>>& q,
                    std::vector<std::vector<bool>>& myVisited,
                    std::vector<std::vector<std::pair<int,int>>>& myParent,
                    std::vector<std::vector<bool>>& otherVisited) {
        if (q.empty()) return false;

        auto [r, c] = q.front();
        q.pop();

        markVisited(grid->getCell(r, c));
        nodesExplored++;

        for (int i = 0; i < 4; i++) {
            int nr = r + DR[i];
            int nc = c + DC[i];

            if (!grid->inBounds(nr, nc)) continue;
            if (myVisited[nr][nc]) continue;
            if (!grid->getCell(nr, nc).isWalkable()) continue;

            myVisited[nr][nc] = true;
            myParent[nr][nc] = {r, c};
            q.push({nr, nc});

            // Collision! Other search already visited this cell
            if (otherVisited[nr][nc]) {
                meetR = nr;
                meetC = nc;
                foundPath = true;
                finished = true;
                reconstructBidirectionalPath();
                return true;
            }
        }
        return false;
    }

    // Custom path reconstruction: merge both halves
    void reconstructBidirectionalPath() {
        if (meetR == -1) return;

        std::vector<std::pair<int,int>> path;

        // Build path from start to meeting point
        int r = meetR, c = meetC;
        std::vector<std::pair<int,int>> fromStart;
        while (r != -1) {
            fromStart.push_back({r, c});
            auto p = parentStart[r][c];
            if (p.first == -1) break;
            r = p.first;
            c = p.second;
        }
        // Reverse it (want start -> meeting)
        for (int i = fromStart.size() - 1; i >= 0; i--) {
            path.push_back(fromStart[i]);
        }

        // Build path from meeting point to end
        r = meetR;
        c = meetC;
        auto next = parentEnd[r][c];
        while (next.first != -1) {
            r = next.first;
            c = next.second;
            path.push_back({r, c});
            next = parentEnd[r][c];
        }

        pathLength = path.size();

        for (auto& [pr, pc] : path) {
            markPath(grid->getCell(pr, pc));
        }
    }
};