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
    void setHover(int row, int col) { hoverRow = row; hoverCol = col; }
    void clearHover() { hoverRow = -1; hoverCol = -1; }

    void render(sf::RenderWindow& window, const Grid& grid) {
        sf::RectangleShape mazeArea(sf::Vector2f(MAZE_WIDTH, MAZE_HEIGHT));
        mazeArea.setPosition(0, 0);
        mazeArea.setFillColor(Colors::BG_DARK);
        window.draw(mazeArea);

        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                drawCell(window, grid.getCell(r, c));
            }
        }
    }

private:
    void drawCell(sf::RenderWindow& window, const Cell& cell) {
    float x = cell.col * CELL_SIZE;
    float y = cell.row * CELL_SIZE;
    float size = CELL_SIZE - 1;

    bool isHovered = (cell.row == hoverRow && cell.col == hoverCol);

    // PRIORITY 1: Start
    if (cell.isStart()) {
        drawGlowCell(window, x, y, size, Colors::START, Colors::START_GLOW, Colors::START_CORE);
        return;
    }

    // PRIORITY 2: End
    if (cell.isEnd()) {
        drawGlowCell(window, x, y, size, Colors::END, Colors::END_GLOW, Colors::END_CORE);
        return;
    }

    // PRIORITY 3: Walls
    if (cell.isWall()) {
        drawWallCell(window, x, y, size);
        return;
    }

    // ============================================
    // TERRAIN AS BASE LAYER
    // Draw terrain FIRST, then overlay algorithm visualization on top
    // This keeps terrain always visible
    // ============================================
    bool hasTerrain = (cell.terrain == Terrain::SAND ||
                       cell.terrain == Terrain::WATER ||
                       cell.terrain == Terrain::MUD);

    if (hasTerrain) {
        drawTerrainCell(window, x, y, size, getTerrainColor(cell), cell.cost);
    } else {
        // Empty cell base
        sf::Color cellColor = isHovered ? Colors::EMPTY_HOVER : Colors::EMPTY;
        drawEmptyCell(window, x, y, size, cellColor);
    }

    // ============================================
    // OVERLAY: Algorithm visualization
    // These are drawn ON TOP but with transparency
    // so terrain stays visible underneath
    // ============================================

    // Path overlap (both BFS and DFS paths)
    if (cell.bfsPath && cell.dfsPath) {
        drawOverlayCell(window, x, y, size, Colors::PATH_OVERLAP, hasTerrain);
        return;
    }

    // Single path
    if (cell.bfsPath)      { drawOverlayCell(window, x, y, size, Colors::BFS_PATH, hasTerrain); return; }
    if (cell.dfsPath)      { drawOverlayCell(window, x, y, size, Colors::DFS_PATH, hasTerrain); return; }
    if (cell.astarPath)    { drawOverlayCell(window, x, y, size, Colors::ASTAR_PATH, hasTerrain); return; }
    if (cell.dijkstraPath) { drawOverlayCell(window, x, y, size, Colors::DIJKSTRA_PATH, hasTerrain); return; }
    if (cell.greedyPath)   { drawOverlayCell(window, x, y, size, Colors::GREEDY_PATH, hasTerrain); return; }
    if (cell.bidirPath)    { drawOverlayCell(window, x, y, size, Colors::BIDIR_PATH, hasTerrain); return; }
    if (cell.qlearnPath)   { drawOverlayCell(window, x, y, size, Colors::QLEARN_PATH, hasTerrain); return; }

    // Frontier
    if (cell.isFrontier) {
        drawOverlayCell(window, x, y, size, Colors::FRONTIER, hasTerrain);
        return;
    }

    // Both visited
    if (cell.bfsVisited && cell.dfsVisited) {
        drawOverlayCell(window, x, y, size, Colors::BOTH_VISITED, hasTerrain);
        return;
    }

    // Single algorithm visited (translucent)
    if (cell.bfsVisited)      { drawOverlayCell(window, x, y, size, Colors::BFS_VISITED, hasTerrain); return; }
    if (cell.dfsVisited)      { drawOverlayCell(window, x, y, size, Colors::DFS_VISITED, hasTerrain); return; }
    if (cell.astarVisited)    { drawOverlayCell(window, x, y, size, Colors::ASTAR_VISITED, hasTerrain); return; }
    if (cell.dijkstraVisited) { drawOverlayCell(window, x, y, size, Colors::DIJKSTRA_VISITED, hasTerrain); return; }
    if (cell.greedyVisited)   { drawOverlayCell(window, x, y, size, Colors::GREEDY_VISITED, hasTerrain); return; }
    if (cell.bidirVisited)    { drawOverlayCell(window, x, y, size, Colors::BIDIR_VISITED, hasTerrain); return; }
}

    void drawEmptyCell(sf::RenderWindow& window, float x, float y, float size, sf::Color color) {
        sf::RectangleShape rect(sf::Vector2f(size, size));
        rect.setPosition(x, y);
        rect.setFillColor(color);
        rect.setOutlineThickness(0.5f);
        rect.setOutlineColor(Colors::GRID_LINE);
        window.draw(rect);
    }

    void drawFilledCell(sf::RenderWindow& window, float x, float y, float size, sf::Color color) {
        sf::RectangleShape rect(sf::Vector2f(size, size));
        rect.setPosition(x, y);
        rect.setFillColor(color);
        window.draw(rect);
    }

    void drawWallCell(sf::RenderWindow& window, float x, float y, float size) {
        sf::RectangleShape wall(sf::Vector2f(size, size));
        wall.setPosition(x, y);
        wall.setFillColor(Colors::WALL);
        window.draw(wall);

        sf::RectangleShape topEdge(sf::Vector2f(size, 2));
        topEdge.setPosition(x, y);
        topEdge.setFillColor(Colors::WALL_HIGHLIGHT);
        window.draw(topEdge);

        sf::RectangleShape leftEdge(sf::Vector2f(2, size));
        leftEdge.setPosition(x, y);
        leftEdge.setFillColor(Colors::WALL_HIGHLIGHT);
        window.draw(leftEdge);

        sf::RectangleShape bottomEdge(sf::Vector2f(size, 2));
        bottomEdge.setPosition(x, y + size - 2);
        bottomEdge.setFillColor(Colors::WALL_SHADOW);
        window.draw(bottomEdge);

        sf::RectangleShape rightEdge(sf::Vector2f(2, size));
        rightEdge.setPosition(x + size - 2, y);
        rightEdge.setFillColor(Colors::WALL_SHADOW);
        window.draw(rightEdge);
    }

    void drawGlowCell(sf::RenderWindow& window, float x, float y, float size,
                       sf::Color main, sf::Color glow, sf::Color core) {
        float glowSize = size + 4;
        sf::RectangleShape glowRect(sf::Vector2f(glowSize, glowSize));
        glowRect.setPosition(x - 2, y - 2);
        glowRect.setFillColor(glow);
        window.draw(glowRect);

        sf::RectangleShape mainRect(sf::Vector2f(size, size));
        mainRect.setPosition(x, y);
        mainRect.setFillColor(main);
        window.draw(mainRect);

        float coreSize = size * 0.4f;
        float offset = (size - coreSize) / 2;
        sf::RectangleShape coreRect(sf::Vector2f(coreSize, coreSize));
        coreRect.setPosition(x + offset, y + offset);
        coreRect.setFillColor(core);
        window.draw(coreRect);
    }

    sf::Color getTerrainColor(const Cell& cell) {
    switch (cell.terrain) {
        case Terrain::SAND:  return Colors::SAND;
        case Terrain::WATER: return Colors::WATER;
        case Terrain::MUD:   return Colors::MUD;
        default: return sf::Color(0, 0, 0, 0);
    }
}

// ============================================
// DRAW TERRAIN CELL WITH COST DOTS
// ============================================
void drawTerrainCell(sf::RenderWindow& window, float x, float y, float size,
                      sf::Color color, int cost) {
    // Main terrain color
    sf::RectangleShape rect(sf::Vector2f(size, size));
    rect.setPosition(x, y);
    rect.setFillColor(color);
    window.draw(rect);

    // Draw small dots to indicate cost (1 dot per cost level, up to 3)
    int dots = std::min(cost - 1, 3);  // sand=2 dots, water=4 dots (max 3), mud=3 dots
    float dotSize = 2.5f;
    float dotSpacing = 4.0f;
    float startX = x + size / 2 - ((dots - 1) * dotSpacing) / 2;
    float dotY = y + size - 6;

    for (int i = 0; i < dots; i++) {
        sf::CircleShape dot(dotSize);
        dot.setPosition(startX + i * dotSpacing - dotSize, dotY);
        dot.setFillColor(sf::Color(255, 255, 255, 180));
        window.draw(dot);
    }
}

// ============================================
// DRAW ALGORITHM OVERLAY ON TOP OF TERRAIN
// If terrain is present, use lower alpha so terrain shows through
// ============================================
void drawOverlayCell(sf::RenderWindow& window, float x, float y, float size,
                      sf::Color color, bool hasTerrainBelow) {
    sf::Color overlayColor = color;

    // If terrain is below, reduce alpha so terrain shows through
    if (hasTerrainBelow) {
        // For paths: keep bright but see-through
        if (overlayColor.a > 200) {
            overlayColor.a = 160;  // brighter for paths
        } else {
            // For visited cells: even more transparent
            overlayColor.a = std::min((int)overlayColor.a, 80);
        }
    }

    sf::RectangleShape rect(sf::Vector2f(size, size));
    rect.setPosition(x, y);
    rect.setFillColor(overlayColor);
    window.draw(rect);
}
};