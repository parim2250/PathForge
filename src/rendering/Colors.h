#pragma once
#include <SFML/Graphics.hpp>

namespace Colors {

    // BACKGROUND — CHARCOAL / PURPLE
    const sf::Color BG_APP       = sf::Color(11, 10, 15);
    const sf::Color BG_PANEL     = sf::Color(19, 16, 27);
    const sf::Color UI_ELEMENT   = sf::Color(31, 26, 42);
    const sf::Color UI_HOVER     = sf::Color(48, 38, 65);

    const sf::Color BORDER_DIM   = sf::Color(75, 57, 94, 120);
    const sf::Color GRID_BG      = sf::Color(9, 8, 13);

    // ACCENTS
    const sf::Color ACCENT_TEAL   = sf::Color(0, 229, 255);
    const sf::Color ACCENT_PURPLE = sf::Color(174, 91, 255);
    const sf::Color ACCENT_GREEN  = sf::Color(57, 255, 143);
    const sf::Color ACCENT_RED    = sf::Color(255, 65, 105);

    // TEXT
    const sf::Color TEXT_PRIMARY   = sf::Color(245, 240, 250);
    const sf::Color TEXT_SECONDARY = sf::Color(170, 160, 185);
    const sf::Color TEXT_MUTED     = sf::Color(110, 101, 125);

    // GRID
    const sf::Color EMPTY       = sf::Color(21, 19, 29);
    const sf::Color EMPTY_HOVER = sf::Color(39, 32, 52);
    const sf::Color GRID_LINE   = sf::Color(11, 10, 15);

    // WALLS
    const sf::Color WALL           = sf::Color(48, 42, 59);
    const sf::Color WALL_HIGHLIGHT = sf::Color(75, 64, 91);
    const sf::Color WALL_SHADOW    = sf::Color(25, 21, 32);

    // START / END
    const sf::Color START      = sf::Color(0, 235, 145);
    const sf::Color START_GLOW = sf::Color(0, 235, 145, 65);
    const sf::Color START_CORE = sf::Color(220, 255, 240);

    const sf::Color END      = sf::Color(255, 70, 105);
    const sf::Color END_GLOW = sf::Color(255, 70, 105, 65);
    const sf::Color END_CORE = sf::Color(255, 225, 230);

    // TERRAIN
    const sf::Color SAND  = sf::Color(225, 165, 65);
    const sf::Color WATER = sf::Color(45, 155, 215);
    const sf::Color MUD   = sf::Color(145, 88, 90);

    // =========================================
    // EXPLORED CELLS — LOW ALPHA NEON TINTS
    // =========================================

    const sf::Color BFS_VISITED      = sf::Color(0, 210, 255, 65);
    const sf::Color DFS_VISITED      = sf::Color(255, 195, 50, 65);
    const sf::Color ASTAR_VISITED    = sf::Color(175, 90, 255, 65);
    const sf::Color DIJKSTRA_VISITED = sf::Color(50, 235, 125, 65);
    const sf::Color GREEDY_VISITED   = sf::Color(255, 65, 155, 65);
    const sf::Color BIDIR_VISITED    = sf::Color(255, 135, 55, 65);
    const sf::Color IDDFS_VISITED    = sf::Color(35, 220, 195, 65);

    const sf::Color BOTH_VISITED =
        sf::Color(190, 125, 230, 85);

    // =========================================
    // FINAL PATH — SOLID, BRIGHT COLOURS
    // =========================================

    const sf::Color BFS_PATH =
        sf::Color(0, 229, 255);

    const sf::Color DFS_PATH =
        sf::Color(255, 205, 35);

    const sf::Color ASTAR_PATH =
        sf::Color(255, 255, 255);

    const sf::Color DIJKSTRA_PATH =
        sf::Color(40, 255, 125);

    const sf::Color GREEDY_PATH =
        sf::Color(255, 60, 172);

    const sf::Color BIDIR_PATH =
        sf::Color(255, 140, 40);

    const sf::Color IDDFS_PATH =
        sf::Color(30, 220, 190);

    const sf::Color QLEARN_PATH =
        sf::Color(0, 229, 255);

    // OVERLAPS
    const sf::Color PATH_OVERLAP =
        sf::Color(255, 215, 55);

    const sf::Color OVERLAP_PATH =
        sf::Color(255, 215, 55);

    const sf::Color OVERLAP_VISITED =
        sf::Color(190, 125, 230, 85);

    const sf::Color FRONTIER =
        sf::Color(170, 145, 195, 100);

    // COMPATIBILITY
    inline const sf::Color BORDER = BORDER_DIM;
    inline const sf::Color BUTTON_BG = UI_ELEMENT;
    inline const sf::Color BUTTON_HOVER = UI_HOVER;
}