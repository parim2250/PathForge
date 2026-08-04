#pragma once
#include <vector>
#include "Cell.h"
#include "Constants.h"

class Grid {
private:
    // 2D array of cells
    std::vector<std::vector<Cell>> cells;

    // Positions of start and end (for quick access)
    int startRow, startCol;
    int endRow, endCol;

public:
    // ============================================
    // CONSTRUCTOR
    // ============================================
    Grid() : startRow(-1), startCol(-1), endRow(-1), endCol(-1) {
        // Initialize 2D grid of cells
        cells.resize(ROWS, std::vector<Cell>(COLS));

        // Set each cell's row and col
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                cells[r][c] = Cell(r, c);
            }
        }
    }

    // ============================================
    // ACCESS CELLS
    // ============================================
    Cell& getCell(int row, int col) {
        return cells[row][col];
    }

    // For read-only access
    const Cell& getCell(int row, int col) const {
        return cells[row][col];
    }

    // Check if position is within bounds
    bool inBounds(int row, int col) const {
        return row >= 0 && row < ROWS && col >= 0 && col < COLS;
    }

    // ============================================
    // START AND END POINT MANAGEMENT
    // ============================================
    void setStart(int row, int col) {
        if (!inBounds(row, col)) return;

        // Remove old start if exists
        if (startRow != -1 && startCol != -1) {
            cells[startRow][startCol].terrain = Terrain::EMPTY;
        }

        // Set new start
        cells[row][col].terrain = Terrain::START;
        startRow = row;
        startCol = col;
    }

    void setEnd(int row, int col) {
        if (!inBounds(row, col)) return;

        // Remove old end if exists
        if (endRow != -1 && endCol != -1) {
            cells[endRow][endCol].terrain = Terrain::EMPTY;
        }

        // Set new end
        cells[row][col].terrain = Terrain::END;
        endRow = row;
        endCol = col;
    }

    // Get start position
    int getStartRow() const { return startRow; }
    int getStartCol() const { return startCol; }
    int getEndRow() const { return endRow; }
    int getEndCol() const { return endCol; }

    bool hasStart() const { return startRow != -1; }
    bool hasEnd() const { return endRow != -1; }

    // ============================================
    // WALL MANAGEMENT
    // ============================================
    void toggleWall(int row, int col) {
        if (!inBounds(row, col)) return;

        Cell& cell = cells[row][col];

        // Do not overwrite start or end
        if (cell.isStart() || cell.isEnd()) return;

        if (cell.terrain == Terrain::WALL) {
            cell.terrain = Terrain::EMPTY;
        } else {
            cell.terrain = Terrain::WALL;
        }
    }

    // Set a wall directly (used for click-and-drag)
    void setWall(int row, int col) {
        if (!inBounds(row, col)) return;
        Cell& cell = cells[row][col];
        if (cell.isStart() || cell.isEnd()) return;
        cell.terrain = Terrain::WALL;
    }

    // ============================================
    // CLEAR OPERATIONS
    // ============================================

    // Clear only algorithm visualization (keep walls, start, end)
    void clearVisuals() {
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                cells[r][c].resetVisual();
            }
        }
    }

    // Clear everything (nuke the whole grid)
    void clearAll() {
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                cells[r][c].reset();
            }
        }
        startRow = startCol = endRow = endCol = -1;
    }

    // ============================================
    // UTILITY
    // ============================================
    int getRows() const { return ROWS; }
    int getCols() const { return COLS; }

    // ============================================
    // GET ALL WALKABLE NEIGHBORS
    // ============================================
    std::vector<std::pair<int,int>> getNeighbors(int row, int col) const {
        std::vector<std::pair<int,int>> neighbors;
        for (int i = 0; i < 4; i++) {
            int nr = row + DR[i];
            int nc = col + DC[i];
            if (inBounds(nr, nc) && cells[nr][nc].isWalkable()) {
                neighbors.push_back({nr, nc});
            }
        }
        return neighbors;
    }
};