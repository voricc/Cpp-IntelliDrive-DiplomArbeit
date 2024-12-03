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
    std::vector<sf::CircleShape> collisionMarkers;

    sf::Vector2i boundaries;

    std::vector<std::vector<int>> placedTileIDs;
    std::vector<std::vector<sf::Sprite>> placedTileSprites;
    std::vector<Tile> tiles;

    sf::Vector2f spawnPointPosition;
    sf::Vector2f spawnPointDirection;
    bool hasSpawnPoint = false;
    float debugTimer = 0.0f;

    void initializeCar();
    void performRaycasts(Game& game);
    void initialiazeRays();
    bool isPauseKeyPressed(const sf::Event& event) const;
    sf::RectangleShape createRoad(Game& game) const;
    bool isPointInPolygon(const sf::Vector2f& point, const sf::ConvexShape& polygon, const sf::Transform& transform);

    bool isPointInPolygon(const sf::Vector2f& point, const sf::ConvexShape& polygon);
};

#endif // GAMESTATE_H
