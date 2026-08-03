#pragma once
#include <SFML/Graphics.hpp>

namespace Colors {
    // ============================================
    // BACKGROUND & UI
    // ============================================
    const sf::Color BG_DARK       = sf::Color(15, 18, 28);     // main window background
    const sf::Color BG_MID        = sf::Color(22, 26, 40);     // panel background
    const sf::Color BG_LIGHT      = sf::Color(30, 35, 50);     // panel highlight
    const sf::Color UI_BG         = sf::Color(15, 18, 28);     // legacy alias
    const sf::Color UI_PANEL      = sf::Color(22, 26, 40);     // legacy alias

    // ============================================
    // CELL BASE COLORS
    // ============================================
    const sf::Color EMPTY         = sf::Color(35, 40, 55);     // empty cell (bluish dark)
    const sf::Color EMPTY_HOVER   = sf::Color(55, 65, 90);     // when mouse hovers
    const sf::Color GRID_LINE     = sf::Color(45, 52, 70);     // subtle grid lines

    // ============================================
    // WALLS (with depth effect)
    // ============================================
    const sf::Color WALL          = sf::Color(85, 95, 120);    // main wall color (much more visible!)
    const sf::Color WALL_HIGHLIGHT= sf::Color(120, 135, 170);  // top-left edge of wall (light)
    const sf::Color WALL_SHADOW   = sf::Color(50, 58, 78);     // bottom-right edge of wall (dark)

    // ============================================
    // START & END (bright and glowing)
    // ============================================
    const sf::Color START         = sf::Color(46, 213, 115);   // vibrant green
    const sf::Color START_GLOW    = sf::Color(46, 213, 115, 80); // semi-transparent glow
    const sf::Color START_CORE    = sf::Color(200, 255, 220);  // bright center

    const sf::Color END           = sf::Color(255, 71, 87);    // vibrant red
    const sf::Color END_GLOW      = sf::Color(255, 71, 87, 80);
    const sf::Color END_CORE      = sf::Color(255, 210, 210);

    // ============================================
    // TERRAIN (weighted cells)
    // ============================================
    const sf::Color SAND          = sf::Color(255, 195, 105);
    const sf::Color WATER         = sf::Color(84, 160, 255);
    const sf::Color MUD           = sf::Color(139, 90, 60);

    // ============================================
    // ALGORITHM VISUALIZATION (bold and distinct)
    // ============================================
    // BFS - Ocean Blue
    const sf::Color BFS_VISITED      = sf::Color(52, 152, 219, 200);
    const sf::Color BFS_PATH         = sf::Color(93, 173, 226);

    // DFS - Sunset Orange
    const sf::Color DFS_VISITED      = sf::Color(230, 126, 34, 200);
    const sf::Color DFS_PATH         = sf::Color(245, 176, 65);

    // A* - Royal Purple
    const sf::Color ASTAR_VISITED    = sf::Color(155, 89, 182, 200);
    const sf::Color ASTAR_PATH       = sf::Color(187, 143, 206);

    // Dijkstra - Emerald Green
    const sf::Color DIJKSTRA_VISITED = sf::Color(46, 204, 113, 200);
    const sf::Color DIJKSTRA_PATH    = sf::Color(88, 214, 141);

    // Greedy - Pink
    const sf::Color GREEDY_VISITED   = sf::Color(236, 64, 122, 200);
    const sf::Color GREEDY_PATH      = sf::Color(244, 143, 177);

    // Bidirectional - Golden Yellow
    const sf::Color BIDIR_VISITED    = sf::Color(241, 196, 15, 200);
    const sf::Color BIDIR_PATH       = sf::Color(247, 220, 111);

    // Q-Learning - Electric Cyan
    const sf::Color QLEARN_PATH      = sf::Color(0, 245, 212);

    // Frontier (cells being explored right now)
    const sf::Color FRONTIER         = sf::Color(255, 235, 59, 220);

    // ============================================
    // UI ELEMENTS
    // ============================================
    const sf::Color TEXT_PRIMARY     = sf::Color(230, 235, 245);
    const sf::Color TEXT_SECONDARY   = sf::Color(150, 160, 180);
    const sf::Color TEXT_ACCENT      = sf::Color(93, 173, 226);
    const sf::Color TEXT             = sf::Color(230, 235, 245);  // legacy alias
    const sf::Color ACCENT           = sf::Color(93, 173, 226);   // legacy alias

    const sf::Color BUTTON_IDLE      = sf::Color(45, 55, 75);
    const sf::Color BUTTON_HOVER     = sf::Color(65, 80, 110);
    const sf::Color BUTTON_ACTIVE    = sf::Color(52, 152, 219);

    const sf::Color BORDER           = sf::Color(60, 70, 90);
    const sf::Color BORDER_LIGHT     = sf::Color(90, 105, 135);
}