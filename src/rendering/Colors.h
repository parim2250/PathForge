#pragma once
#include <SFML/Graphics.hpp>

namespace Colors {
    // ============================================
    // PREMIUM DARK UI THEME
    // ============================================
    const sf::Color BG_APP          = sf::Color(10, 14, 23);
    const sf::Color BG_PANEL        = sf::Color(15, 23, 36);
    const sf::Color UI_ELEMENT      = sf::Color(30, 41, 59);
    const sf::Color UI_HOVER        = sf::Color(51, 65, 85);
    const sf::Color BORDER_DIM      = sf::Color(51, 65, 85, 150);
    const sf::Color GRID_BG         = sf::Color(15, 23, 36);

    const sf::Color ACCENT_TEAL     = sf::Color(0, 245, 212);
    const sf::Color ACCENT_PURPLE   = sf::Color(139, 92, 246);
    const sf::Color ACCENT_GREEN    = sf::Color(46, 213, 115);
    const sf::Color ACCENT_RED      = sf::Color(255, 71, 87);

    const sf::Color TEXT_PRIMARY    = sf::Color(241, 245, 249);
    const sf::Color TEXT_SECONDARY  = sf::Color(148, 163, 184);
    const sf::Color TEXT_MUTED      = sf::Color(100, 116, 139);

    const sf::Color EMPTY           = sf::Color(25, 35, 50);
    const sf::Color EMPTY_HOVER     = sf::Color(35, 45, 65);
    const sf::Color GRID_LINE       = sf::Color(40, 50, 70);

    const sf::Color WALL            = sf::Color(71, 85, 105);
    const sf::Color WALL_HIGHLIGHT  = sf::Color(100, 116, 139);
    const sf::Color WALL_SHADOW     = sf::Color(30, 41, 59);

    // START = GREEN, END = RED
    const sf::Color START           = sf::Color(46, 213, 115);
    const sf::Color START_GLOW      = sf::Color(46, 213, 115, 90);
    const sf::Color START_CORE      = sf::Color(200, 255, 220);

    const sf::Color END             = sf::Color(255, 71, 87);
    const sf::Color END_GLOW        = sf::Color(255, 71, 87, 90);
    const sf::Color END_CORE        = sf::Color(255, 210, 210);

    // Distinct terrain colors
    const sf::Color SAND            = sf::Color(234, 179, 8);
    const sf::Color WATER           = sf::Color(56, 189, 248);
    const sf::Color MUD             = sf::Color(168, 100, 84);

    const sf::Color BFS_VISITED      = sf::Color(56, 189, 248, 80);
    const sf::Color DFS_VISITED      = sf::Color(250, 204, 21, 80);
    const sf::Color ASTAR_VISITED    = sf::Color(192, 132, 252, 80);
    const sf::Color DIJKSTRA_VISITED = sf::Color(74, 222, 128, 80);
    const sf::Color GREEDY_VISITED   = sf::Color(251, 113, 133, 80);
    const sf::Color BIDIR_VISITED    = sf::Color(251, 146, 60, 80);
    const sf::Color BOTH_VISITED     = sf::Color(148, 163, 184, 100);

    const sf::Color BFS_PATH         = sf::Color(56, 189, 248);
    const sf::Color DFS_PATH         = sf::Color(250, 204, 21);
    const sf::Color ASTAR_PATH       = sf::Color(192, 132, 252);
    const sf::Color DIJKSTRA_PATH    = sf::Color(74, 222, 128);
    const sf::Color GREEDY_PATH      = sf::Color(251, 113, 133);
    const sf::Color BIDIR_PATH       = sf::Color(251, 146, 60);
    const sf::Color QLEARN_PATH      = sf::Color(0, 245, 212);

    const sf::Color PATH_OVERLAP     = ACCENT_TEAL;
    const sf::Color FRONTIER         = sf::Color(255, 255, 255, 180);
    const sf::Color IDDFS_VISITED = sf::Color(45, 212, 191, 80);
const sf::Color IDDFS_PATH    = sf::Color(45, 212, 191);
}