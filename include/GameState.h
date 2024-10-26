//
// Created by Voric and tobisdev on 11/08/2024.
//

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "../include/State.h"
#include "../include/Car.h"
#include "../include/Game.h"
#include "../include/ResourceManager.h"
#include <cmath>

class GameState : public State {
public:
    GameState(Game& game, const std::string& levelFile);
    void GameStateBackground(Game& game);

    GameState(Game& game) : car(game.getCar()) {
        initializeCar();
    }

    void handleInput(Game& game) override;
    void loadLevelFromCSV(const std::string &filename, Game &game);

    void update(Game& game) override;
    void render(Game& game) override;

private:
    Car& car;
    float timeSinceLastPrint = 0.0f;
    sf::Sprite backgroundSprite;

    std::vector<float> rayDistances;
    std::vector<sf::VertexArray> rays;
    std::vector<sf::VertexArray> collisionMarkers;
    std::vector<sf::RectangleShape> walls;

    sf::Vector2i boundaries; // How many tiles can be placed on the x -and y-axis

    std::vector<std::vector<int>> placedTileIDs;
    std::vector<std::vector<sf::Sprite>> placedTileSprites;
    std::vector<Tile> tiles;

    void debugDrawing(Game& game);
    void initializeCar();
    void performRaycasts(Game& game);
    void initialiazeRays();
    bool isPauseKeyPressed(const sf::Event& event) const;
    sf::RectangleShape createRoad(Game& game) const;
};

#endif // GAMESTATE_H
