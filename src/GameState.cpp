//
// Created by Voric and tobisdev on 11/08/2024.
//

# include "GameState.h"

GameState::GameState(Game &game, const std::string &levelFile) : car(game.getCar()), GameStateParent(game, levelFile) {
    initializeCar();
}

void GameState::initializeCar() {
    bool hasSpawnPoint = this->getHasSpawnpoint();
    auto &spawnPointPosition = this->getSpawnPointPosition();
    auto &spawnPointDirection = this->getSpawnPointDirection();

    std::cout << "[DEBUG] Initializing car\n";
    sf::Sprite &carSprite = car.getCarSprite();
    carSprite.setOrigin(carSprite.getLocalBounds().width / 2, carSprite.getLocalBounds().height / 2);

    if (hasSpawnPoint) {
        std::cout << "[DEBUG] Setting car position and rotation from spawn point\n";
        carSprite.setPosition(spawnPointPosition);
        car.resetVelocity();
        car.resetAngularAcceleration();

        float angle = std::atan2(spawnPointDirection.y, spawnPointDirection.x) * 180.f / M_PI + 90.f;
        car.setRotationAngle(angle);

        car.setPreviousPosition(spawnPointPosition);
        car.setCurrentPosition(spawnPointPosition);
    } else {
        std::cout << "[DEBUG] No spawn point data, using default position\n";
        carSprite.setPosition(400, 400);
        car.resetVelocity();
        car.resetAngularAcceleration();
        car.resetRotationAngle();
        car.setPreviousPosition({carSprite.getPosition().x, carSprite.getPosition().y});
        car.setCurrentPosition({carSprite.getPosition().x, carSprite.getPosition().y});
    }
}

void GameState::render(Game &game) {
    sf::Vector2i &boundaries = this->getBoundaries();
    auto &backgroundSprite = this->getBackgroundSprite();
    auto &placedTileIDs = this->getPlacedTileIDs();
    auto &placedTileSprites = this->getPlacedTileSprites();
    auto &tiles = this->getTiles();

    game.window.clear();
    game.window.draw(backgroundSprite);

    for (int x = 0; x < boundaries.x; ++x) {
        for (int y = 0; y < boundaries.y; ++y) {
            game.window.draw(placedTileSprites[x][y]);

            int tileID = placedTileIDs[x][y];
            if (tileID >= 0) {
                Tile& tile = tiles[tileID];
                sf::ConvexShape collisionShape = tile.collisionShape;
            }
        }
    }

    car.render(game.window);
}

void GameState::update(Game &game) {
    sf::Vector2i &boundaries = this->getBoundaries();
    auto &placedTileIDs = this->getPlacedTileIDs();
    auto &placedTileSprites = this->getPlacedTileSprites();
    auto &tiles = this->getTiles();

    car.update(game.dt);

    // Get the car's transformed points
    sf::Transform carTransform = car.getCarSprite().getTransform();
    sf::FloatRect carLocalBounds = car.getCarSprite().getLocalBounds();

    std::vector<sf::Vector2f> carPoints(4);
    carPoints[0] = carTransform.transformPoint(sf::Vector2f(carLocalBounds.left, carLocalBounds.top));
    carPoints[1] = carTransform.transformPoint(sf::Vector2f(carLocalBounds.left + carLocalBounds.width, carLocalBounds.top));
    carPoints[2] = carTransform.transformPoint(sf::Vector2f(carLocalBounds.left + carLocalBounds.width, carLocalBounds.top + carLocalBounds.height));
    carPoints[3] = carTransform.transformPoint(sf::Vector2f(carLocalBounds.left, carLocalBounds.top + carLocalBounds.height));

    bool allPointsOnRoad = true;

    for (const auto& point : carPoints) {
        bool pointOnRoad = false;

        // Determine which tile the point is over
        int tileSize = VariableManager::getTileSize();
        sf::Vector2i pointTilePosition(
                static_cast<int>(point.x / tileSize),
                static_cast<int>(point.y / tileSize)
        );

        // Search neighboring tiles
        int searchRadius = 1;
        for (int dx = -searchRadius; dx <= searchRadius; ++dx) {
            for (int dy = -searchRadius; dy <= searchRadius; ++dy) {
                int x = pointTilePosition.x + dx;
                int y = pointTilePosition.y + dy;

                if (x < 0 || x >= boundaries.x || y < 0 || y >= boundaries.y)
                    continue;

                int tileID = placedTileIDs[x][y];
                if (tileID < 0) continue;

                Tile& tile = tiles[tileID];
                const sf::ConvexShape& collisionShape = tile.collisionShape;

                sf::Sprite& tileSprite = placedTileSprites[x][y];

                sf::Transform combinedTransform = tileSprite.getTransform() * collisionShape.getTransform();

                if (isPointInPolygon(point, collisionShape, combinedTransform)) {
                    pointOnRoad = true;
                    break;
                }
            }
            if (pointOnRoad) break;
        }

        if (!pointOnRoad) {
            allPointsOnRoad = false;
            break; // One point is off the road
        }
    }

    if (!allPointsOnRoad) {
        std::cout << "[DEBUG] Car is off the road\n";
        game.changeState(std::make_shared<DeathState>());
        return;
    }
}

void GameState::handleInput(Game &game) {
    sf::Event event;
    while (game.window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            std::cout << "[DEBUG] Window closed event received\n";
            game.window.close();
        }
        if (isPauseKeyPressed(event)) {
            std::cout << "[DEBUG] Pause key pressed\n";
            game.pushState(std::make_shared<PauseState>());
        }
    }

    // Handle car movement input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        car.setAcceleration(car.getAccelerationConstant());
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        car.setAcceleration(-car.getAccelerationConstant());
    } else {
        car.setAcceleration(0.0f);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        car.setAngularAcceleration(-car.getAngularAccelerationConstant());
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        car.setAngularAcceleration(car.getAngularAccelerationConstant());
    } else {
        car.setAngularAcceleration(0.0f);
    }
}
