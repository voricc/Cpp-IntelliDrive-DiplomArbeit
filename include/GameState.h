//
// Created by Voric and tobisdev on 11/08/2024.
//

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "GameStateParent.h"
#include <vector>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>

class GameState : public GameStateParent {
private:
    std::vector<float> rayDistances;
    std::vector<sf::VertexArray> rays;
    std::vector<sf::CircleShape> collisionMarkers;

    void initializeCar() override;
    void initializeRays() override;
    void performRaycasts(Game &game) override;

    void render(Game &game) override;
    void update(Game &game) override;
    void handleInput(Game &game) override;
public:
    GameState(Game &game, const std::string &levelFile);

    explicit GameState(Game &game) : GameStateParent(game) {
        initializeCar();
    }
};

#endif // GAMESTATE_H


