//
// Created by Voric on 11/08/2024.
//

#include "SFML/Window/Keyboard.hpp"
#include <vector>
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

#ifndef CAR_H
#define CAR_H

class Car {
public:
    Car();

    void handleInput();
    void update();
    void render(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;

private:
    sf::RectangleShape carShape;
    float speed;
    sf::Vector2f direction;
};

#endif //CAR_H
