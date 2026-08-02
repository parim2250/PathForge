#pragma once
#include <SFML/Graphics.hpp>

namespace Colors {
    const sf::Color EMPTY        = sf::Color(40, 40, 40);
    const sf::Color WALL         = sf::Color(20, 20, 20);
    const sf::Color START        = sf::Color(0, 255, 100);
    const sf::Color END          = sf::Color(255, 50, 50);

    const sf::Color SAND         = sf::Color(210, 180, 140);
    const sf::Color WATER        = sf::Color(30, 144, 255);
    const sf::Color MUD          = sf::Color(101, 67, 33);

    const sf::Color BFS_VISITED      = sf::Color(0, 120, 255, 180);
    const sf::Color BFS_PATH         = sf::Color(0, 200, 255);
    const sf::Color DFS_VISITED      = sf::Color(255, 100, 0, 180);
    const sf::Color DFS_PATH         = sf::Color(255, 180, 0);
    const sf::Color ASTAR_VISITED    = sf::Color(150, 0, 255, 180);
    const sf::Color ASTAR_PATH       = sf::Color(200, 100, 255);
    const sf::Color DIJKSTRA_VISITED = sf::Color(0, 200, 100, 180);
    const sf::Color DIJKSTRA_PATH    = sf::Color(0, 255, 150);
    const sf::Color GREEDY_VISITED   = sf::Color(255, 0, 150, 180);
    const sf::Color GREEDY_PATH      = sf::Color(255, 100, 200);
    const sf::Color BIDIR_VISITED    = sf::Color(255, 255, 0, 180);
    const sf::Color BIDIR_PATH       = sf::Color(255, 255, 100);
    const sf::Color QLEARN_PATH      = sf::Color(100, 255, 255);

    const sf::Color FRONTIER     = sf::Color(255, 255, 0, 200);

    const sf::Color UI_BG        = sf::Color(25, 25, 35);
    const sf::Color UI_PANEL     = sf::Color(35, 35, 50);
    const sf::Color BUTTON_IDLE  = sf::Color(60, 60, 80);
    const sf::Color BUTTON_HOVER = sf::Color(80, 80, 110);
    const sf::Color BUTTON_ACTIVE= sf::Color(100, 100, 150);
    const sf::Color TEXT         = sf::Color(220, 220, 220);
    const sf::Color ACCENT       = sf::Color(100, 200, 255);

    const sf::Color GRID_LINE    = sf::Color(50, 50, 60);
}