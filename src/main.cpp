#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    std::cout << "Step 1: Program started" << std::endl;
    std::cout.flush();

    std::cout << "Step 2: Creating window..." << std::endl;
    std::cout.flush();
    
    sf::RenderWindow window(
        sf::VideoMode(800, 600), 
        "PathForge - Setup Test"
    );

    std::cout << "Step 3: Window created successfully" << std::endl;
    std::cout.flush();

    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color(100, 200, 255));
    shape.setPosition(300, 200);

    std::cout << "Step 4: Shape created. Entering main loop..." << std::endl;
    std::cout.flush();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color(25, 25, 35));
        window.draw(shape);
        window.display();
    }

    std::cout << "Step 5: Window closed. Exiting." << std::endl;
    return 0;
}