#pragma once
#include <SFML/Graphics.hpp>
#include "../core/Grid.h"
#include "../core/Constants.h"
#include "Colors.h"

class MazeRenderer {
private:
    int hoverRow = -1;
    int hoverCol = -1;

public:
    // ============================================
    // SET WHICH CELL THE MOUSE IS HOVERING
    // ============================================
    void setHover(int row, int col) {
        hoverRow = row;
        hoverCol = col;
    }

    void clearHover() {
        hoverRow = -1;
        hoverCol = -1;
    }

    // ============================================
    // MAIN RENDER FUNCTION
    // ============================================
    void render(sf::RenderWindow& window, const Grid& grid) {
        // Draw maze area background (subtle base layer)
        sf::RectangleShape mazeArea(sf::Vector2f(MAZE_WIDTH, MAZE_HEIGHT));
        mazeArea.setPosition(0, 0);
        mazeArea.setFillColor(Colors::BG_DARK);
        window.draw(mazeArea);

        // Draw all cells
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                drawCell(window, grid.getCell(r, c));
            }
        }
    }

private:
    // ============================================
    // DRAW A SINGLE CELL
    // ============================================
    void drawCell(sf::RenderWindow& window, const Cell& cell) {
        float x = cell.col * CELL_SIZE;
        float y = cell.row * CELL_SIZE;
        float size = CELL_SIZE - 1;   // -1 for grid line effect

        bool isHovered = (cell.row == hoverRow && cell.col == hoverCol);

        // ==============================
        // START POINT (with glow)
        // ==============================
        if (cell.isStart()) {
            drawGlowCell(window, x, y, size, Colors::START, Colors::START_GLOW, Colors::START_CORE);
            return;
        }

        // ==============================
        // END POINT (with glow)
        // ==============================
        if (cell.isEnd()) {
            drawGlowCell(window, x, y, size, Colors::END, Colors::END_GLOW, Colors::END_CORE);
            return;
        }

        // ==============================
        // WALLS (with 3D depth effect)
        // ==============================
        if (cell.isWall()) {
            drawWallCell(window, x, y, size);
            return;
        }

        // ==============================
        // ALGORITHM VISUALIZATION
        // ==============================
        sf::Color visualColor = getVisualColor(cell);
        if (visualColor.a > 0) {
            drawFilledCell(window, x, y, size, visualColor);
            return;
        }

        // ==============================
        // TERRAIN (sand, water, mud)
        // ==============================
        sf::Color terrainColor = getTerrainColor(cell);
        if (terrainColor.a > 0) {
            drawFilledCell(window, x, y, size, terrainColor);
            return;
        }

        // ==============================
        // EMPTY CELL (with hover effect)
        // ==============================
        sf::Color cellColor = isHovered ? Colors::EMPTY_HOVER : Colors::EMPTY;
        drawEmptyCell(window, x, y, size, cellColor);
    }

    // ============================================
    // DRAW EMPTY CELL WITH SUBTLE BORDER
    // ============================================
    void drawEmptyCell(sf::RenderWindow& window, float x, float y, float size, sf::Color color) {
        sf::RectangleShape rect(sf::Vector2f(size, size));
        rect.setPosition(x, y);
        rect.setFillColor(color);
        rect.setOutlineThickness(0.5f);
        rect.setOutlineColor(Colors::GRID_LINE);
        window.draw(rect);
    }

    // ============================================
    // DRAW FILLED CELL (for visited/path)
    // ============================================
    void drawFilledCell(sf::RenderWindow& window, float x, float y, float size, sf::Color color) {
        sf::RectangleShape rect(sf::Vector2f(size, size));
        rect.setPosition(x, y);
        rect.setFillColor(color);
        window.draw(rect);
    }

    // ============================================
    // DRAW WALL WITH 3D DEPTH EFFECT
    // ============================================
    void drawWallCell(sf::RenderWindow& window, float x, float y, float size) {
        // Main wall body
        sf::RectangleShape wall(sf::Vector2f(size, size));
        wall.setPosition(x, y);
        wall.setFillColor(Colors::WALL);
        window.draw(wall);

        // Top edge (light) — gives 3D raised look
        sf::RectangleShape topEdge(sf::Vector2f(size, 2));
        topEdge.setPosition(x, y);
        topEdge.setFillColor(Colors::WALL_HIGHLIGHT);
        window.draw(topEdge);

        // Left edge (light)
        sf::RectangleShape leftEdge(sf::Vector2f(2, size));
        leftEdge.setPosition(x, y);
        leftEdge.setFillColor(Colors::WALL_HIGHLIGHT);
        window.draw(leftEdge);

        // Bottom edge (dark)
        sf::RectangleShape bottomEdge(sf::Vector2f(size, 2));
        bottomEdge.setPosition(x, y + size - 2);
        bottomEdge.setFillColor(Colors::WALL_SHADOW);
        window.draw(bottomEdge);

        // Right edge (dark)
        sf::RectangleShape rightEdge(sf::Vector2f(2, size));
        rightEdge.setPosition(x + size - 2, y);
        rightEdge.setFillColor(Colors::WALL_SHADOW);
        window.draw(rightEdge);
    }

    // ============================================
    // DRAW START/END CELL WITH GLOW EFFECT
    // ============================================
    void drawGlowCell(sf::RenderWindow& window, float x, float y, float size,
                       sf::Color main, sf::Color glow, sf::Color core) {
        // Outer glow (larger, transparent)
        float glowSize = size + 4;
        sf::RectangleShape glowRect(sf::Vector2f(glowSize, glowSize));
        glowRect.setPosition(x - 2, y - 2);
        glowRect.setFillColor(glow);
        window.draw(glowRect);

        // Main color
        sf::RectangleShape mainRect(sf::Vector2f(size, size));
        mainRect.setPosition(x, y);
        mainRect.setFillColor(main);
        window.draw(mainRect);

        // Bright core (smaller square in center)
        float coreSize = size * 0.4f;
        float offset = (size - coreSize) / 2;
        sf::RectangleShape coreRect(sf::Vector2f(coreSize, coreSize));
        coreRect.setPosition(x + offset, y + offset);
        coreRect.setFillColor(core);
        window.draw(coreRect);
    }

    // ============================================
    // GET COLOR FOR ALGORITHM VISUALIZATION
    // ============================================
    sf::Color getVisualColor(const Cell& cell) {
        switch (cell.visualState) {
            case VisualState::VISITED_BFS:      return Colors::BFS_VISITED;
            case VisualState::VISITED_DFS:      return Colors::DFS_VISITED;
            case VisualState::VISITED_ASTAR:    return Colors::ASTAR_VISITED;
            case VisualState::VISITED_DIJKSTRA: return Colors::DIJKSTRA_VISITED;
            case VisualState::VISITED_GREEDY:   return Colors::GREEDY_VISITED;
            case VisualState::VISITED_BIDIR:    return Colors::BIDIR_VISITED;
            case VisualState::FRONTIER:         return Colors::FRONTIER;
            case VisualState::PATH_BFS:         return Colors::BFS_PATH;
            case VisualState::PATH_DFS:         return Colors::DFS_PATH;
            case VisualState::PATH_ASTAR:       return Colors::ASTAR_PATH;
            case VisualState::PATH_DIJKSTRA:    return Colors::DIJKSTRA_PATH;
            case VisualState::PATH_GREEDY:      return Colors::GREEDY_PATH;
            case VisualState::PATH_BIDIR:       return Colors::BIDIR_PATH;
            case VisualState::PATH_QLEARN:      return Colors::QLEARN_PATH;
            default: return sf::Color(0, 0, 0, 0);
        }
    }

    // ============================================
    // GET COLOR FOR TERRAIN (weighted cells)
    // ============================================
    sf::Color getTerrainColor(const Cell& cell) {
        switch (cell.terrain) {
            case Terrain::SAND:  return Colors::SAND;
            case Terrain::WATER: return Colors::WATER;
            case Terrain::MUD:   return Colors::MUD;
            default: return sf::Color(0, 0, 0, 0);
        }
    }
};