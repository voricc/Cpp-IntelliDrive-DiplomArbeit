//
// Created by Voric and tobisdev on 11/08/2024.
//

#ifndef AIGAMESTATE_H
#define AIGAMESTATE_H

#include "GameStateParent.h"
#include <vector>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "../vendors/ai/NeuralNetwork/NeuralNetwork.h"

class AiGameState : public GameStateParent {
private:
    std::vector<float> rayDistances;
    std::vector<sf::VertexArray> rays;
    std::vector<sf::CircleShape> collisionMarkers;

    NeuralNetwork network;

    void initializeCar() override;
    void initializeRays() override;
    void performRaycasts(Game &game) override;

    void render(Game &game) override;
    void update(Game &game) override;
    void handleInput(Game &game) override;

    void updateAI();
public:
    AiGameState(Game &game, const std::string &levelFile);

    explicit AiGameState(Game &game) : GameStateParent(game) {
        initializeCar();
    }
};

#endif // GAMESTATE_H


