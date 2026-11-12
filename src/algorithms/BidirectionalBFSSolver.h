#pragma once
#include "Solver.h"
#include <queue>

// ============================================
// BIDIRECTIONAL BFS
// Two BFS frontiers (Start + End) meet in the middle.
// Layer-based expansion preserves unweighted shortest-path guarantee.
// ============================================

class BidirectionalBFSSolver : public Solver {
private:
    std::queue<std::pair<int,int>> frontierStart;
    std::queue<std::pair<int,int>> frontierEnd;

    std::vector<std::vector<bool>> visitedStart;
    std::vector<std::vector<bool>> visitedEnd;
    std::vector<std::vector<std::pair<int,int>>> parentStart;
    std::vector<std::vector<std::pair<int,int>>> parentEnd;

    int meetR = -1, meetC = -1;
    bool searchFromStart = true;  // alternate layers each step()

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

        // CHANGE 2: validate ALL four coordinates (not only sr/er)
        if (sr == -1 || sc == -1 || er == -1 || ec == -1) {
            finished = true;
            return;
        }

        // CHANGE: trivial maze (start == end)
        if (sr == er && sc == ec) {
            meetR = sr;
            meetC = sc;
            foundPath = true;
            finished = true;
            pathLength = 1;  // same convention as path.size() for a single cell
            pathCost = 0;    // no traversal away from start
            return;
        }

        frontierStart.push({sr, sc});
        frontierEnd.push({er, ec});
        visitedStart[sr][sc] = true;
        visitedEnd[er][ec] = true;
    }

    std::string getName() const override { return "BiDir BFS"; }

    bool step() override {
        if (finished) return false;
        if (frontierStart.empty() && frontierEnd.empty()) {
            finished = true;
            return false;
        }

        // CHANGE 3: expand one FULL BFS LAYER on the active side, then alternate.
        // Snapshot size so newly pushed neighbors belong to the NEXT layer only.
        // This restores the unweighted shortest-path guarantee of bidirectional BFS.
        // Single-node alternation is NOT sufficient for that guarantee.
        if (searchFromStart && !frontierStart.empty()) {
            int layerSize = static_cast<int>(frontierStart.size());
            for (int i = 0; i < layerSize && !finished; i++) {
                if (expandOne(frontierStart, visitedStart, parentStart, visitedEnd)) {
                    return false; // path found + reconstructed (or rejected)
                }
            }
        } else if (!searchFromStart && !frontierEnd.empty()) {
            int layerSize = static_cast<int>(frontierEnd.size());
            for (int i = 0; i < layerSize && !finished; i++) {
                if (expandOne(frontierEnd, visitedEnd, parentEnd, visitedStart)) {
                    return false;
                }
            }
        } else if (!frontierStart.empty()) {
            // Active side empty but the other still has nodes — drain remaining side in layers
            int layerSize = static_cast<int>(frontierStart.size());
            for (int i = 0; i < layerSize && !finished; i++) {
                if (expandOne(frontierStart, visitedStart, parentStart, visitedEnd)) {
                    return false;
                }
            }
        } else if (!frontierEnd.empty()) {
            int layerSize = static_cast<int>(frontierEnd.size());
            for (int i = 0; i < layerSize && !finished; i++) {
                if (expandOne(frontierEnd, visitedEnd, parentEnd, visitedStart)) {
                    return false;
                }
            }
        }

        searchFromStart = !searchFromStart;
        return !finished;
    }

private:
    // Expand one cell from a direction. Returns true if a meeting was accepted
    // (finished is set). Returns true also if meeting was rejected after validation
    // only when finished was set inside reconstruct; here we return whether path search should stop.
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

            // Collision with the opposite search
            if (otherVisited[nr][nc]) {
                meetR = nr;
                meetC = nc;
                reconstructBidirectionalPath(); // validates chains; sets foundPath/finished
                return finished; // true if accepted (or finished); step() stops when finished
            }
        }
        return false;
    }

    // CHANGE 1: validate parentStart reaches real start AND parentEnd reaches real end
    // BEFORE marking path or publishing metrics. No incomplete path leftovers.
    void reconstructBidirectionalPath() {
        if (meetR == -1) {
            foundPath = false;
            return;
        }

        int sr = grid->getStartRow();
        int sc = grid->getStartCol();
        int er = grid->getEndRow();
        int ec = grid->getEndCol();

        // --- Build meeting -> start via parentStart ---
        std::vector<std::pair<int,int>> fromStart;
        int r = meetR, c = meetC;
        while (r != -1 && c != -1) {
            fromStart.push_back({r, c});
            if (r == sr && c == sc) break;
            auto p = parentStart[r][c];
            r = p.first;
            c = p.second;
        }

        // Validate: chain must end at the actual start
        if (fromStart.empty() ||
            !(fromStart.back().first == sr && fromStart.back().second == sc)) {
            foundPath = false;
            finished = true; // search met but path invalid — stop cleanly
            pathLength = 0;
            pathCost = 0;
            meetR = meetC = -1;
            return;
        }

        // --- Build meeting -> end via parentEnd (exclude meeting; already in fromStart) ---
        std::vector<std::pair<int,int>> toEnd;
        if (!(meetR == er && meetC == ec)) {
            r = meetR;
            c = meetC;
            auto next = parentEnd[r][c];
            while (next.first != -1 && next.second != -1) {
                r = next.first;
                c = next.second;
                toEnd.push_back({r, c});
                if (r == er && c == ec) break;
                next = parentEnd[r][c];
            }
        }

        // Validate: either meeting is the end, or chain ends at the actual end
        bool endReached =
            (meetR == er && meetC == ec) ||
            (!toEnd.empty() && toEnd.back().first == er && toEnd.back().second == ec);

        if (!endReached) {
            foundPath = false;
            finished = true;
            pathLength = 0;
            pathCost = 0;
            meetR = meetC = -1;
            return;
        }

        // --- Merge: start ... meeting ... end ---
        std::vector<std::pair<int,int>> path;
        for (int i = static_cast<int>(fromStart.size()) - 1; i >= 0; i--) {
            path.push_back(fromStart[i]);
        }
        for (auto& cell : toEnd) {
            path.push_back(cell);
        }

        // Accepted only after full validation — now mark + metrics
        foundPath = true;
        finished = true;
        pathLength = static_cast<int>(path.size()); // preserved convention
        pathCost = 0;

        for (auto& [pr, pc] : path) {
            if (!(pr == sr && pc == sc)) {
                pathCost += grid->getCell(pr, pc).cost;
            }
            markPath(grid->getCell(pr, pc));
        }
    }
};