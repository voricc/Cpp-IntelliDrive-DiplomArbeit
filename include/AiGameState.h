//
// Created by Voric and tobisdev on 11/08/2024.
//

#ifndef AIGAMESTATE_H
#define AIGAMESTATE_H

#include "GameStateParent.h"
#include "ResourceManager.h"
#include <vector>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "../vendors/ai/NeuralNetwork/NeuralNetwork.h"
#include "Car.h"

enum Debug : int {None = 0, Checkpoints = 1, Rays = 2, Hitboxes = 3, BestCar = 4};

struct Player {
    std::vector<float> rayDistances;
    std::vector<sf::VertexArray> rays;
    std::vector<sf::CircleShape> collisionMarkers;
    int nextCheckpoint = 0;
    bool isDead = false;
    float points = 0.0f;
    Car car;
};

class AiGameState : public GameStateParent {
private:
    std::vector<Player> players;
    carData carTemplate;

    bool forceReset = false;

    Debug debug = None;

    int currentGen = 0;
    int deadCars = 0;
    float mutationIndex = 0.04f;

    float resetDeadPercentage = 0.995f;

    int networks = 5000;
    NeuralNetwork network;

    sf::Font textFont;

    std::vector<sf::Vector2f> checkpoints;
    float checkpointRadius = 140.0f;

    void initializeCar() override;
    void initializeRays() override;
    void performRaycasts(Game &game) override;

    void render(Game &game) override;
    void update(Game &game) override;
    void handleInput(Game &game) override;

    void updateAI();
public:
    AiGameState(Game &game, const std::string &levelFile, bool debugMode = false);

    explicit AiGameState(Game &game) : GameStateParent(game) {
        initializeCar();
    }
};

#endif // GAMESTATE_H


