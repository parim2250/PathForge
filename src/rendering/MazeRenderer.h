#pragma once

#include <SFML/Graphics.hpp>
#include <algorithm>

#include "../core/Grid.h"
#include "../core/Constants.h"
#include "Colors.h"

class MazeRenderer {

private:
    int hoverRow = -1;
    int hoverCol = -1;

public:

    void setHover(int row, int col) {
        hoverRow = row;
        hoverCol = col;
    }

    void clearHover() {
        hoverRow = -1;
        hoverCol = -1;
    }

    void render(sf::RenderWindow& window, const Grid& grid) {

        // ============================================
        // APPLICATION BACKGROUND
        // ============================================

        sf::RectangleShape appBg(
            sf::Vector2f(
                static_cast<float>(window.getSize().x),
                static_cast<float>(window.getSize().y)
            )
        );

        appBg.setPosition(0, 0);
        appBg.setFillColor(Colors::BG_APP);
        window.draw(appBg);

        // Subtle purple ambient lighting
        sf::CircleShape ambientGlow(260.f);
        ambientGlow.setPosition(
            GRID_OFFSET_X + COLS * CELL_SIZE * 0.25f - 260.f,
            GRID_OFFSET_Y + ROWS * CELL_SIZE * 0.25f - 260.f
        );
        ambientGlow.setFillColor(sf::Color(85, 35, 125, 12));
        window.draw(ambientGlow);

        sf::CircleShape secondGlow(220.f);
        secondGlow.setPosition(
            GRID_OFFSET_X + COLS * CELL_SIZE * 0.7f - 220.f,
            GRID_OFFSET_Y + ROWS * CELL_SIZE * 0.65f - 220.f
        );
        secondGlow.setFillColor(sf::Color(30, 45, 110, 10));
        window.draw(secondGlow);

        // ============================================
        // GRID CONTAINER
        // ============================================

        sf::RectangleShape gridContainer(
            sf::Vector2f(
                COLS * CELL_SIZE + 20,
                ROWS * CELL_SIZE + 20
            )
        );

        gridContainer.setPosition(
            GRID_OFFSET_X - 10,
            GRID_OFFSET_Y - 10
        );

        gridContainer.setFillColor(Colors::GRID_BG);
        gridContainer.setOutlineThickness(1);
        gridContainer.setOutlineColor(Colors::BORDER_DIM);

        window.draw(gridContainer);

        // ============================================
        // DRAW GRID CELLS
        // ============================================

        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                drawCell(window, grid.getCell(r, c));
            }
        }
    }

private:

    void drawCell(
        sf::RenderWindow& window,
        const Cell& cell
    ) {

        float x = GRID_OFFSET_X + cell.col * CELL_SIZE;
        float y = GRID_OFFSET_Y + cell.row * CELL_SIZE;

        // One-pixel gap between tiles
        float size = CELL_SIZE - 1;

        bool isHovered =
            (cell.row == hoverRow && cell.col == hoverCol);

        // ============================================
        // START AND END CELLS
        // ============================================

        if (cell.isStart()) {
            drawGlowCell(
                window, x, y, size,
                Colors::START,
                Colors::START_GLOW,
                Colors::START_CORE
            );
            return;
        }

        if (cell.isEnd()) {
            drawGlowCell(
                window, x, y, size,
                Colors::END,
                Colors::END_GLOW,
                Colors::END_CORE
            );
            return;
        }

        // ============================================
        // WALLS
        // ============================================

        if (cell.isWall()) {
            drawWallCell(window, x, y, size);
            return;
        }

        // ============================================
        // TERRAIN AND EMPTY CELLS
        // ============================================

        bool hasTerrain =
            (cell.terrain == Terrain::SAND ||
             cell.terrain == Terrain::WATER ||
             cell.terrain == Terrain::MUD);

        if (hasTerrain) {

            drawTerrainCell(
                window,
                x,
                y,
                size,
                getTerrainColor(cell),
                cell.cost
            );

        }
        else {

            sf::Color cellColor =
                isHovered ? Colors::EMPTY_HOVER : Colors::EMPTY;

            drawEmptyCell(
                window,
                x,
                y,
                size,
                cellColor
            );
        }

        // ============================================
        // FINAL PATH OVERLAP DETECTION
        // ============================================

        int pathCount =
            (int)cell.bfsPath +
            (int)cell.dfsPath +
            (int)cell.astarPath +
            (int)cell.dijkstraPath +
            (int)cell.greedyPath +
            (int)cell.bidirPath +
            (int)cell.iddfsPath;

        // Multiple algorithms share this path cell
        if (pathCount >= 2) {

            drawOverlayCell(
                window, x, y, size,
                Colors::OVERLAP_PATH,
                false
            );

            return;
        }

        // Individual algorithm paths
        if (pathCount == 1) {

            if (cell.bfsPath) {
                drawOverlayCell(window, x, y, size, Colors::BFS_PATH, false);
                return;
            }

            if (cell.dfsPath) {
                drawOverlayCell(window, x, y, size, Colors::DFS_PATH, false);
                return;
            }

            if (cell.astarPath) {
                drawOverlayCell(window, x, y, size, Colors::ASTAR_PATH, false);
                return;
            }

            if (cell.dijkstraPath) {
                drawOverlayCell(window, x, y, size, Colors::DIJKSTRA_PATH, false);
                return;
            }

            if (cell.greedyPath) {
                drawOverlayCell(window, x, y, size, Colors::GREEDY_PATH, false);
                return;
            }

            if (cell.bidirPath) {
                drawOverlayCell(window, x, y, size, Colors::BIDIR_PATH, false);
                return;
            }

            if (cell.iddfsPath) {
                drawOverlayCell(window, x, y, size, Colors::IDDFS_PATH, false);
                return;
            }
        }

        // ============================================
        // FRONTIER CELLS
        // ============================================

        if (cell.isFrontier) {

            drawOverlayCell(
                window,
                x,
                y,
                size,
                Colors::FRONTIER,
                hasTerrain
            );

            return;
        }

        // ============================================
        // EXPLORED CELL OVERLAP DETECTION
        // ============================================

        int visCount =
            (int)cell.bfsVisited +
            (int)cell.dfsVisited +
            (int)cell.astarVisited +
            (int)cell.dijkstraVisited +
            (int)cell.greedyVisited +
            (int)cell.bidirVisited +
            (int)cell.iddfsVisited;

        // Multiple algorithms explored this cell
        if (visCount >= 2) {

            drawOverlayCell(
                window,
                x,
                y,
                size,
                Colors::OVERLAP_VISITED,
                hasTerrain
            );

            return;
        }

        // Individual exploration highlights
        if (visCount == 1) {

            if (cell.bfsVisited) {
                drawOverlayCell(window, x, y, size, Colors::BFS_VISITED, hasTerrain);
                return;
            }

            if (cell.dfsVisited) {
                drawOverlayCell(window, x, y, size, Colors::DFS_VISITED, hasTerrain);
                return;
            }

            if (cell.astarVisited) {
                drawOverlayCell(window, x, y, size, Colors::ASTAR_VISITED, hasTerrain);
                return;
            }

            if (cell.dijkstraVisited) {
                drawOverlayCell(window, x, y, size, Colors::DIJKSTRA_VISITED, hasTerrain);
                return;
            }

            if (cell.greedyVisited) {
                drawOverlayCell(window, x, y, size, Colors::GREEDY_VISITED, hasTerrain);
                return;
            }

            if (cell.bidirVisited) {
                drawOverlayCell(window, x, y, size, Colors::BIDIR_VISITED, hasTerrain);
                return;
            }

            if (cell.iddfsVisited) {
                drawOverlayCell(window, x, y, size, Colors::IDDFS_VISITED, hasTerrain);
                return;
            }
        }
    }

    // ============================================
    // EMPTY CELL
    // ============================================

    void drawEmptyCell(
        sf::RenderWindow& window,
        float x,
        float y,
        float size,
        sf::Color color
    ) {

        sf::RectangleShape rect(
            sf::Vector2f(size, size)
        );

        rect.setPosition(x, y);
        rect.setFillColor(color);

        window.draw(rect);
    }

    // ============================================
    // WALL CELL
    // ============================================

    void drawWallCell(
        sf::RenderWindow& window,
        float x,
        float y,
        float size
    ) {

        sf::RectangleShape wall(
            sf::Vector2f(size, size)
        );

        wall.setPosition(x, y);
        wall.setFillColor(Colors::WALL);

        window.draw(wall);
    }

    // ============================================
    // START / END GLOW
    // ============================================

    void drawGlowCell(
        sf::RenderWindow& window,
        float x,
        float y,
        float size,
        sf::Color main,
        sf::Color glow,
        sf::Color core
    ) {

        sf::RectangleShape glowRect(
            sf::Vector2f(size + 4, size + 4)
        );

        glowRect.setPosition(x - 2, y - 2);
        glowRect.setFillColor(glow);

        window.draw(glowRect);

        sf::RectangleShape mainRect(
            sf::Vector2f(size, size)
        );

        mainRect.setPosition(x, y);
        mainRect.setFillColor(main);

        window.draw(mainRect);

        sf::RectangleShape coreRect(
            sf::Vector2f(size * 0.4f, size * 0.4f)
        );

        coreRect.setPosition(
            x + size * 0.3f,
            y + size * 0.3f
        );

        coreRect.setFillColor(core);

        window.draw(coreRect);
    }

    // ============================================
    // TERRAIN COLOURS
    // ============================================

    sf::Color getTerrainColor(const Cell& cell) {

        switch (cell.terrain) {

            case Terrain::SAND:
                return Colors::SAND;

            case Terrain::WATER:
                return Colors::WATER;

            case Terrain::MUD:
                return Colors::MUD;

            default:
                return sf::Color::Transparent;
        }
    }

    // ============================================
    // TERRAIN CELL
    // ============================================

    void drawTerrainCell(
        sf::RenderWindow& window,
        float x,
        float y,
        float size,
        sf::Color color,
        int cost
    ) {

        sf::RectangleShape rect(
            sf::Vector2f(size, size)
        );

        rect.setPosition(x, y);
        rect.setFillColor(color);

        window.draw(rect);

        int dots = std::min(cost - 1, 3);

        for (int i = 0; i < dots; i++) {

            sf::CircleShape dot(2.0f);

            dot.setPosition(
                x + (size / 2) - (dots * 3) + (i * 6),
                y + size - 5
            );

            dot.setFillColor(
                sf::Color(255, 255, 255, 180)
            );

            window.draw(dot);
        }
    }

    // ============================================
    // OVERLAY CELL
    // ============================================

    void drawOverlayCell(
        sf::RenderWindow& window,
        float x,
        float y,
        float size,
        sf::Color color,
        bool hasTerrainBelow
    ) {

        // Explored cells remain subtle over terrain.
        // Final paths pass false and remain fully opaque.

        if (hasTerrainBelow) {
            color.a = 55;
        }

        sf::RectangleShape rect(
            sf::Vector2f(size, size)
        );

        rect.setPosition(x, y);
        rect.setFillColor(color);

        window.draw(rect);
    }
};