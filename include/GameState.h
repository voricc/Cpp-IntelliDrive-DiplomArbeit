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
    Car &car;

    void initializeCar() override;

    void render(Game &game) override;
    void update(Game &game) override;
    void handleInput(Game &game) override;
public:
    GameState(Game &game, const std::string &levelFile, bool debugMode = false);

    explicit GameState(Game &game) : car(game.getCar()), GameStateParent(game) {
        initializeCar();
    }
};

#endif // GAMESTATE_H


