#pragma once
#include "Solver.h"
#include <queue>

// ============================================
// DIJKSTRA'S ALGORITHM
// Like BFS but supports cell costs (weights)
// Uses priority queue based on distance (no heuristic)
// Perfect for weighted terrain (Part 4)
// ============================================

class DijkstraSolver : public Solver {
private:
    struct Node {
        int row, col;
        int distance;

        bool operator>(const Node& other) const {
            return distance > other.distance;
        }
    };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> frontier;
    std::vector<std::vector<int>> distance;

public:
    DijkstraSolver(Grid* g) : Solver(g, Algorithm::DIJKSTRA) {
        int rows = grid->getRows();
        int cols = grid->getCols();
        distance.assign(rows, std::vector<int>(cols, INT_MAX));

        int sr = grid->getStartRow();
        int sc = grid->getStartCol();

        if (sr != -1 && sc != -1) {
            distance[sr][sc] = 0;
            frontier.push({sr, sc, 0});
        } else {
            finished = true;
        }
    }

    std::string getName() const override { return "Dijkstra"; }

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

            // Use cell cost (default 1, but can be higher for terrain)
            int cellCost = grid->getCell(nr, nc).cost;
            int newDist = current.distance + cellCost;

            if (newDist < distance[nr][nc]) {
                distance[nr][nc] = newDist;
                parent[nr][nc] = {current.row, current.col};
                frontier.push({nr, nc, newDist});
            }
        }

        return true;
    }
};