#pragma once
#include "../core/Grid.h"
#include <queue>
#include <vector>
#include <algorithm>
#include <string>

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

    // Score 0-100 tuned for 31x51 presets (Easy/Medium/Hard separation)
    static int calculateDifficulty(const AnalysisResult& r) {
        if (!r.hasPath) return 0;
        int totalCells = r.openCells + r.wallCells;
        if (totalCells <= 0) return 0;
        // 1) Forced path length (0-35)
        // Easy ~80-140, Medium ~150-250, Hard ~280-450
        int pathScore = std::min(35, (r.optimalPathLength * 35) / 400);
        // 2) Wall density (0-25)
        double wallRatio = (double)r.wallCells / (double)totalCells;
        int wallScore = std::min(25, (int)(wallRatio * 55.0));
        // 3) Dead-end count (0-25)
        // Easy ~0-15, Hard ~30-80+
        int deadEndScore = std::min(25, (r.deadEnds * 25) / 55);
        // 4) How “tight” the optimal path is vs open space (0-15)
        // Higher = long snake through the grid (harder)
        int tightScore = 0;
        if (r.openCells > 0) {
            double pathFrac = (double)r.optimalPathLength / (double)r.openCells;
            tightScore = std::min(15, (int)(pathFrac * 90.0));
        }
        int score = pathScore + wallScore + deadEndScore + tightScore;
        if (score < 0) score = 0;
        if (score > 100) score = 100;
        return score;
    }

    static std::string getRating(int score) {
        if (score < 28) return "Easy";
        if (score < 52) return "Medium";
        if (score < 75) return "Hard";
        return "Extreme";
    }
};