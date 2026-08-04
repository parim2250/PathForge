#pragma once
#include <SFML/Graphics.hpp>

namespace Colors {
    // ============================================
    // BACKGROUND & UI
    // ============================================
    const sf::Color BG_DARK       = sf::Color(15, 18, 28);
    const sf::Color BG_MID        = sf::Color(22, 26, 40);
    const sf::Color BG_LIGHT      = sf::Color(30, 35, 50);
    const sf::Color UI_BG         = sf::Color(15, 18, 28);
    const sf::Color UI_PANEL      = sf::Color(22, 26, 40);

    // ============================================
    // CELL BASE
    // ============================================
    const sf::Color EMPTY         = sf::Color(35, 40, 55);
    const sf::Color EMPTY_HOVER   = sf::Color(55, 65, 90);
    const sf::Color GRID_LINE     = sf::Color(45, 52, 70);

    // ============================================
    // WALLS
    // ============================================
    const sf::Color WALL          = sf::Color(85, 95, 120);
    const sf::Color WALL_HIGHLIGHT= sf::Color(120, 135, 170);
    const sf::Color WALL_SHADOW   = sf::Color(50, 58, 78);

    // ============================================
    // START & END
    // ============================================
    const sf::Color START         = sf::Color(46, 213, 115);
    const sf::Color START_GLOW    = sf::Color(46, 213, 115, 80);
    const sf::Color START_CORE    = sf::Color(200, 255, 220);

    const sf::Color END           = sf::Color(255, 71, 87);
    const sf::Color END_GLOW      = sf::Color(255, 71, 87, 80);
    const sf::Color END_CORE      = sf::Color(255, 210, 210);

    // ============================================
    // TERRAIN
    // ============================================
    const sf::Color SAND          = sf::Color(255, 195, 105);
    const sf::Color WATER         = sf::Color(84, 160, 255);
    const sf::Color MUD           = sf::Color(139, 90, 60);

    // ============================================
    // ALGORITHM: VISITED (very light/translucent)
    // ============================================
    const sf::Color BFS_VISITED      = sf::Color(93, 173, 226, 70);
    const sf::Color DFS_VISITED      = sf::Color(245, 176, 65, 70);
    const sf::Color ASTAR_VISITED    = sf::Color(187, 143, 206, 70);
    const sf::Color DIJKSTRA_VISITED = sf::Color(88, 214, 141, 70);
    const sf::Color GREEDY_VISITED   = sf::Color(244, 143, 177, 70);
    const sf::Color BIDIR_VISITED    = sf::Color(247, 220, 111, 70);

    // ============================================
    // BOTH VISITED (BFS + DFS overlap)
    // Soft purple/lavender - blend of blue + orange
    // ============================================
    const sf::Color BOTH_VISITED     = sf::Color(180, 140, 220, 120);

    // ============================================
    // ALGORITHM: PATH (bright and clear)
    // ============================================
    const sf::Color BFS_PATH         = sf::Color(64, 196, 255);      // bright cyan
    const sf::Color DFS_PATH         = sf::Color(255, 193, 7);       // bright golden
    const sf::Color ASTAR_PATH       = sf::Color(206, 147, 216);
    const sf::Color DIJKSTRA_PATH    = sf::Color(102, 187, 106);
    const sf::Color GREEDY_PATH      = sf::Color(240, 98, 146);
    const sf::Color BIDIR_PATH       = sf::Color(255, 235, 59);
    const sf::Color QLEARN_PATH      = sf::Color(0, 245, 212);

    // ============================================
    // PATH OVERLAP (both BFS and DFS agree here)
    // Vibrant emerald mint - stunning highlight
    // ============================================
    const sf::Color PATH_OVERLAP     = sf::Color(0, 255, 170);

    // ============================================
    // FRONTIER
    // ============================================
    const sf::Color FRONTIER         = sf::Color(255, 235, 59, 220);

    // ============================================
    // UI ELEMENTS
    // ============================================
    const sf::Color TEXT_PRIMARY     = sf::Color(230, 235, 245);
    const sf::Color TEXT_SECONDARY   = sf::Color(150, 160, 180);
    const sf::Color TEXT_ACCENT      = sf::Color(93, 173, 226);
    const sf::Color TEXT             = sf::Color(230, 235, 245);
    const sf::Color ACCENT           = sf::Color(93, 173, 226);

    const sf::Color BUTTON_IDLE      = sf::Color(45, 55, 75);
    const sf::Color BUTTON_HOVER     = sf::Color(65, 80, 110);
    const sf::Color BUTTON_ACTIVE    = sf::Color(52, 152, 219);

    const sf::Color BORDER           = sf::Color(60, 70, 90);
    const sf::Color BORDER_LIGHT     = sf::Color(90, 105, 135);
}