#pragma once
#include "../core/Grid.h"
#include <queue>
#include <vector>
#include <algorithm>
#include<string>

// ============================================
// ANALYZES A MAZE AND CALCULATES DIFFICULTY
// ============================================

class MazeAnalyzer {
public:
    struct AnalysisResult {
        bool hasPath;
        int optimalPathLength;   // shortest S to E
        int nodesExplored;       // total cells BFS visits
        int deadEnds;            // cells with only 1 walkable neighbor
        int openCells;           // total walkable cells
        int wallCells;           // total wall cells
        int difficultyScore;     // 0-100 scale
        std::string difficultyRating; // "Easy", "Medium", "Hard", "Extreme"
    };

    // ============================================
    // FULL ANALYSIS
    // ============================================
    static AnalysisResult analyze(const Grid& grid) {
        AnalysisResult result{false, 0, 0, 0, 0, 0, 0, "Unknown"};

        int rows = grid.getRows();
        int cols = grid.getCols();

        // Count wall and open cells
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (grid.getCell(r, c).isWalkable()) {
                    result.openCells++;
                    if (countWalkableNeighbors(grid, r, c) == 1) {
                        result.deadEnds++;
                    }
                } else {
                    result.wallCells++;
                }
            }
        }

        // Run BFS to find path length and count visited
        int sr = grid.getStartRow();
        int sc = grid.getStartCol();
        int er = grid.getEndRow();
        int ec = grid.getEndCol();

        if (sr == -1 || er == -1) return result;

        std::vector<std::vector<int>> dist(rows, std::vector<int>(cols, -1));
        std::queue<std::pair<int,int>> q;
        q.push({sr, sc});
        dist[sr][sc] = 0;

        while (!q.empty()) {
            auto [r, c] = q.front();
            q.pop();
            result.nodesExplored++;

            for (int i = 0; i < 4; i++) {
                int nr = r + DR[i];
                int nc = c + DC[i];
                if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;
                if (dist[nr][nc] != -1) continue;
                if (!grid.getCell(nr, nc).isWalkable()) continue;

                dist[nr][nc] = dist[r][c] + 1;
                q.push({nr, nc});
            }
        }

        if (dist[er][ec] != -1) {
            result.hasPath = true;
            result.optimalPathLength = dist[er][ec];
        }

        // Calculate difficulty score (0-100)
        result.difficultyScore = calculateDifficulty(result);
        result.difficultyRating = getRating(result.difficultyScore);

        return result;
    }

private:
    static int countWalkableNeighbors(const Grid& grid, int r, int c) {
        int count = 0;
        for (int i = 0; i < 4; i++) {
            int nr = r + DR[i];
            int nc = c + DC[i];
            if (grid.inBounds(nr, nc) && grid.getCell(nr, nc).isWalkable()) count++;
        }
        return count;
    }

    // Score based on multiple factors
    static int calculateDifficulty(const AnalysisResult& r) {
        if (!r.hasPath) return 0;

        // Base score from path length (longer = harder)
        int pathScore = std::min(40, r.optimalPathLength / 3);

        // Score from dead ends (more dead ends = harder for DFS)
        int deadEndScore = std::min(30, r.deadEnds * 2);

        // Score from nodes explored ratio (more exploration = harder)
        int explorationScore = 0;
        if (r.openCells > 0) {
            double ratio = (double)r.nodesExplored / r.openCells;
            explorationScore = std::min(30, (int)(ratio * 30));
        }

        return pathScore + deadEndScore + explorationScore;
    }

    static std::string getRating(int score) {
        if (score < 25) return "Easy";
        if (score < 50) return "Medium";
        if (score < 75) return "Hard";
        return "Extreme";
    }
};