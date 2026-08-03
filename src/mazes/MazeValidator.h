#pragma once
#include "../core/Grid.h"
#include <queue>
#include <vector>

// ============================================
// VALIDATES THAT A MAZE HAS A VALID PATH
// FROM START TO END USING BFS
// ============================================

class MazeValidator {
public:
    // Returns true if there exists a path from start to end
    static bool hasValidPath(const Grid& grid) {
        int sr = grid.getStartRow();
        int sc = grid.getStartCol();
        int er = grid.getEndRow();
        int ec = grid.getEndCol();

        // Must have both start and end
        if (sr == -1 || er == -1) return false;

        int rows = grid.getRows();
        int cols = grid.getCols();

        std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
        std::queue<std::pair<int,int>> q;
        q.push({sr, sc});
        visited[sr][sc] = true;

        while (!q.empty()) {
            auto [r, c] = q.front();
            q.pop();

            if (r == er && c == ec) return true;

            for (int i = 0; i < 4; i++) {
                int nr = r + DR[i];
                int nc = c + DC[i];

                if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;
                if (visited[nr][nc]) continue;
                if (!grid.getCell(nr, nc).isWalkable()) continue;

                visited[nr][nc] = true;
                q.push({nr, nc});
            }
        }

        return false;
    }
};

