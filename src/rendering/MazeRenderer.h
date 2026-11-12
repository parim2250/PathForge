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
        // App Background
        sf::RectangleShape appBg(sf::Vector2f(MAZE_WIDTH, MAZE_HEIGHT));
        appBg.setPosition(0, 0);
        appBg.setFillColor(Colors::BG_APP);
        window.draw(appBg);

        // Grid Container Background
        sf::RectangleShape gridContainer(sf::Vector2f(COLS * CELL_SIZE + 20, ROWS * CELL_SIZE + 20));
        gridContainer.setPosition(GRID_OFFSET_X - 10, GRID_OFFSET_Y - 10);
        gridContainer.setFillColor(Colors::GRID_BG);
        gridContainer.setOutlineThickness(1);
        gridContainer.setOutlineColor(Colors::BORDER_DIM);
        window.draw(gridContainer);

        // Draw Cells
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                drawCell(window, grid.getCell(r, c));
            }
        }
    }

private:
    void drawCell(sf::RenderWindow& window, const Cell& cell) {
        float x = GRID_OFFSET_X + cell.col * CELL_SIZE;
        float y = GRID_OFFSET_Y + cell.row * CELL_SIZE;
        float size = CELL_SIZE - 1;

        bool isHovered = (cell.row == hoverRow && cell.col == hoverCol);

        if (cell.isStart()) { drawGlowCell(window, x, y, size, Colors::START, Colors::START_GLOW, Colors::START_CORE); return; }
        if (cell.isEnd()) { drawGlowCell(window, x, y, size, Colors::END, Colors::END_GLOW, Colors::END_CORE); return; }
        if (cell.isWall()) { drawWallCell(window, x, y, size); return; }

        bool hasTerrain = (cell.terrain == Terrain::SAND || cell.terrain == Terrain::WATER || cell.terrain == Terrain::MUD);

        if (hasTerrain) {
            drawTerrainCell(window, x, y, size, getTerrainColor(cell), cell.cost);
        } else {
            sf::Color cellColor = isHovered ? Colors::EMPTY_HOVER : Colors::EMPTY;
            drawEmptyCell(window, x, y, size, cellColor);
        }

        // 1. Dynamic Overlap Detection for SOLID PATHS
        int pathCount = (int)cell.bfsPath + (int)cell.dfsPath + (int)cell.astarPath + 
                         (int)cell.dijkstraPath + (int)cell.greedyPath + (int)cell.bidirPath + (int)cell.iddfsPath;
        if (pathCount >= 2) {
            drawOverlayCell(window, x, y, size, Colors::OVERLAP_PATH, hasTerrain); 
            return; 
        } else if (pathCount == 1) {
            if (cell.bfsPath)      { drawOverlayCell(window, x, y, size, Colors::BFS_PATH, hasTerrain); return; }
            if (cell.dfsPath)      { drawOverlayCell(window, x, y, size, Colors::DFS_PATH, hasTerrain); return; }
            if (cell.astarPath)    { drawOverlayCell(window, x, y, size, Colors::ASTAR_PATH, hasTerrain); return; }
            if (cell.dijkstraPath) { drawOverlayCell(window, x, y, size, Colors::DIJKSTRA_PATH, hasTerrain); return; }
            if (cell.greedyPath)   { drawOverlayCell(window, x, y, size, Colors::GREEDY_PATH, hasTerrain); return; }
            if (cell.bidirPath)    { drawOverlayCell(window, x, y, size, Colors::BIDIR_PATH, hasTerrain); return; }
            if (cell.iddfsPath)    { drawOverlayCell(window, x, y, size, Colors::IDDFS_PATH, hasTerrain); return; }
        }
        
        if (cell.isFrontier) { drawOverlayCell(window, x, y, size, Colors::FRONTIER, hasTerrain); return; }

        // 2. Dynamic Overlap Detection for VISITED TINTS
        int visCount = (int)cell.bfsVisited + (int)cell.dfsVisited + (int)cell.astarVisited + 
                        (int)cell.dijkstraVisited + (int)cell.greedyVisited + (int)cell.bidirVisited + (int)cell.iddfsVisited;
        if (visCount >= 2) {
            drawOverlayCell(window, x, y, size, Colors::OVERLAP_VISITED, hasTerrain); 
            return; 
        } else if (visCount == 1) {
            if (cell.bfsVisited)      { drawOverlayCell(window, x, y, size, Colors::BFS_VISITED, hasTerrain); return; }
            if (cell.dfsVisited)      { drawOverlayCell(window, x, y, size, Colors::DFS_VISITED, hasTerrain); return; }
            if (cell.astarVisited)    { drawOverlayCell(window, x, y, size, Colors::ASTAR_VISITED, hasTerrain); return; }
            if (cell.dijkstraVisited) { drawOverlayCell(window, x, y, size, Colors::DIJKSTRA_VISITED, hasTerrain); return; }
            if (cell.greedyVisited)   { drawOverlayCell(window, x, y, size, Colors::GREEDY_VISITED, hasTerrain); return; }
            if (cell.bidirVisited)    { drawOverlayCell(window, x, y, size, Colors::BIDIR_VISITED, hasTerrain); return; }
            if (cell.iddfsVisited)    { drawOverlayCell(window, x, y, size, Colors::IDDFS_VISITED, hasTerrain); return; }
        }
    }

    void drawEmptyCell(sf::RenderWindow& window, float x, float y, float size, sf::Color color) {
        sf::RectangleShape rect(sf::Vector2f(size, size));
        rect.setPosition(x, y);
        rect.setFillColor(color);
        rect.setOutlineThickness(0.5f);
        rect.setOutlineColor(Colors::GRID_LINE);
        window.draw(rect);
    }

    void drawWallCell(sf::RenderWindow& window, float x, float y, float size) {
        sf::RectangleShape wall(sf::Vector2f(size, size));
        wall.setPosition(x, y);
        wall.setFillColor(Colors::WALL);
        window.draw(wall);
    }

    void drawGlowCell(sf::RenderWindow& window, float x, float y, float size, sf::Color main, sf::Color glow, sf::Color core) {
        sf::RectangleShape glowRect(sf::Vector2f(size + 4, size + 4));
        glowRect.setPosition(x - 2, y - 2);
        glowRect.setFillColor(glow);
        window.draw(glowRect);

        sf::RectangleShape mainRect(sf::Vector2f(size, size));
        mainRect.setPosition(x, y);
        mainRect.setFillColor(main);
        window.draw(mainRect);

        sf::RectangleShape coreRect(sf::Vector2f(size * 0.4f, size * 0.4f));
        coreRect.setPosition(x + (size * 0.3f), y + (size * 0.3f));
        coreRect.setFillColor(core);
        window.draw(coreRect);
    }

    sf::Color getTerrainColor(const Cell& cell) {
        switch (cell.terrain) {
            case Terrain::SAND: return Colors::SAND;
            case Terrain::WATER: return Colors::WATER;
            case Terrain::MUD: return Colors::MUD;
            default: return sf::Color::Transparent;
        }
    }

    void drawTerrainCell(sf::RenderWindow& window, float x, float y, float size, sf::Color color, int cost) {
        sf::RectangleShape rect(sf::Vector2f(size, size));
        rect.setPosition(x, y);
        rect.setFillColor(color);
        window.draw(rect);

        int dots = std::min(cost - 1, 3);
        for (int i = 0; i < dots; i++) {
            sf::CircleShape dot(2.0f);
            dot.setPosition(x + (size/2) - (dots*3) + (i*6), y + size - 5);
            dot.setFillColor(sf::Color(255, 255, 255, 180));
            window.draw(dot);
        }
    }

    void drawOverlayCell(sf::RenderWindow& window, float x, float y, float size, sf::Color color, bool hasTerrainBelow) {
        if (hasTerrainBelow) color.a = std::min((int)color.a, 120);
        sf::RectangleShape rect(sf::Vector2f(size, size));
        rect.setPosition(x, y);
        rect.setFillColor(color);
        window.draw(rect);
    }
};