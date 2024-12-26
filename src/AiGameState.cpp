//
// Created by Voric and tobisdev on 11/08/2024.
//

# include "AiGameState.h"

AiGameState::AiGameState(Game &game, const std::string &levelFile, bool debugMode) : GameStateParent(game, levelFile, debugMode) {
    carTemplate = game.cars[0];

    std::vector<int> topology = {5, 3, 2};
    std::vector<Utility::Activations> activations = {
            Utility::Activations::Tanh,
            Utility::Activations::Tanh
    };

    network = NeuralNetwork(topology, activations, -1.8f, +1.8f, true, networks);

    this->initializeCar();
    this->initializeRays();
}

void AiGameState::initializeCar() {
    bool hasSpawnPoint = this->getHasSpawnpoint();
    std::cout << "[DEBUG] Initializing cars\n";

    auto &spawnPointPosition = this->getSpawnPointPosition();
    auto &spawnPointDirection = this->getSpawnPointDirection();
    for (int playerIDX = 0; playerIDX < networks; ++playerIDX) {
        Player player;
        
        player.car = {};
        player.car.applyData(carTemplate);
        Car &car = player.car;

        sf::Sprite &carSprite = car.getCarSprite();
        carSprite.setOrigin(carSprite.getLocalBounds().width / 2, carSprite.getLocalBounds().height / 2);

        if (hasSpawnPoint) {
            carSprite.setPosition(spawnPointPosition);
            car.resetVelocity();
            car.resetAngularAcceleration();

            float angle = std::atan2(spawnPointDirection.y, spawnPointDirection.x) * 180.f / M_PI + 90.f;
            car.setRotationAngle(angle);

            car.setPreviousPosition(spawnPointPosition);
            car.setCurrentPosition(spawnPointPosition);
        } else {
            carSprite.setPosition(400, 400);
            car.resetVelocity();
            car.resetAngularAcceleration();
            car.resetRotationAngle();
            car.setPreviousPosition({carSprite.getPosition().x, carSprite.getPosition().y});
            car.setCurrentPosition({carSprite.getPosition().x, carSprite.getPosition().y});
        }

        players.emplace_back(player);
    }
}

void AiGameState::initializeRays() {
    auto &rayAngles = this->getRayAngles();
    int raySize = rayAngles.size();

    for (int playerIDX = 0; playerIDX < players.size(); ++playerIDX) {
        Player &player = players[playerIDX];
        player.rayDistances = std::vector<float>(raySize, 0.0f);
        player.rays = std::vector<sf::VertexArray>(raySize, {});
        player.collisionMarkers = std::vector<sf::CircleShape>(raySize, {});
    }
}

void AiGameState::performRaycasts(Game &game) {
    sf::Vector2i &boundaries = this->getBoundaries();
    auto &placedTileIDs = this->getPlacedTileIDs();
    auto &placedTileSprites = this->getPlacedTileSprites();
    auto &tiles = this->getTiles();
    bool debugMode = this->getDebugMode();

    for (int playerIDX = 0; playerIDX < networks; ++playerIDX) {
        auto rayAngles = this->getRayAngles();

        Player &player = players[playerIDX];
        Car &car = player.car;
        auto &collisionMarkers = player.collisionMarkers;
        auto &rays = player.rays;
        auto &rayDistances = player.rayDistances;

        float rotation_angle = car.getRotationAngle();
        sf::Vector2f carPosition = car.getCurrentPosition();

        // Normalize angles between 0 and 360 degrees
        for (auto& angle : rayAngles) {
            angle = fmod(angle + rotation_angle + 360.0f, 360.0f);
        }

        collisionMarkers.clear();
        rays.clear();

        sf::Vector2u windowSize = game.window.getSize();
        int tileSize = game.getTileSize();

        for (size_t i = 0; i < rayAngles.size(); ++i) {
            float angle = rayAngles[i];
            float radian_angle = angle * (M_PI / 180.0f);
            sf::Vector2f direction(std::sin(radian_angle), -std::cos(radian_angle));

            sf::Vector2f rayStart = carPosition;
            sf::Vector2f rayEnd = carPosition;

            float stepSize = 5.0f; // Adjust step size for accuracy and performance

            bool rayTerminated = false;
            while (!rayTerminated) {
                rayEnd += direction * stepSize;

                // Check if rayEnd is outside window bounds
                if (rayEnd.x < 0 || rayEnd.x >= windowSize.x || rayEnd.y < 0 || rayEnd.y >= windowSize.y) {
                    rayTerminated = true;
                    break;
                }

                // Determine which tile the rayEnd is over
                int tileX = static_cast<int>(rayEnd.x / tileSize);
                int tileY = static_cast<int>(rayEnd.y / tileSize);

                // Check if tileX and tileY are within boundaries
                if (tileX < 0 || tileX >= boundaries.x || tileY < 0 || tileY >= boundaries.y) {
                    rayTerminated = true;
                    break;
                }

                int tileID = placedTileIDs[tileX][tileY];

                // If there is no tile at this position, terminate the ray
                if (tileID == -1) {
                    rayTerminated = true;
                    break;
                }

                // Get the tile and its collision shape
                Tile& tile = tiles[tileID];
                const sf::ConvexShape& collisionShape = tile.collisionShape;
                sf::Sprite& tileSprite = placedTileSprites[tileX][tileY];

                // Apply the tile sprite's transform to the collision shape
                sf::Transform combinedTransform = tileSprite.getTransform();

                // Check if the point is within the collision shape of the tile
                if (!isPointInPolygon(rayEnd, collisionShape, combinedTransform)) {
                    // The point is in a transparent area (e.g., outside the road), terminate the ray
                    rayTerminated = true;
                    break;
                }

                // Optional: To prevent the ray from getting stuck due to minimal movement, add a maximum length
                float distance = sqrtf(
                        (rayEnd.x - carPosition.x) * (rayEnd.x - carPosition.x) +
                        (rayEnd.y - carPosition.y) * (rayEnd.y - carPosition.y)
                );
                if (distance >= 10000.0f) { // Adjust maximum length as needed
                    rayTerminated = true;
                    break;
                }
            }

            float distance = sqrtf(
                    (rayEnd.x - carPosition.x) * (rayEnd.x - carPosition.x) +
                    (rayEnd.y - carPosition.y) * (rayEnd.y - carPosition.y)
            );
            rayDistances[i] = distance;

            // Create the ray visual representation
            if (debugMode) {
                sf::VertexArray ray(sf::Lines, 2);
                ray[0].position = carPosition;
                ray[0].color = sf::Color(0, 255, 0, 255);
                ray[1].position = rayEnd;
                ray[1].color = sf::Color(0, 255, 0, 255);
                rays.push_back(ray);

                // Add a marker at the collision point
                collisionMarkers.emplace_back();
                sf::CircleShape& marker = collisionMarkers.back();
                marker.setRadius(5);
                marker.setPosition(rayEnd - sf::Vector2f(5, 5));
                marker.setFillColor(sf::Color::Red);
            }
        }
    }
}

void AiGameState::render(Game &game) {
    std::cout << "Rendering!\n";
    sf::Vector2i &boundaries = this->getBoundaries();
    auto &backgroundSprite = this->getBackgroundSprite();
    auto &placedTileIDs = this->getPlacedTileIDs();
    auto &placedTileSprites = this->getPlacedTileSprites();
    auto &tiles = this->getTiles();
    bool debugMode = this->getDebugMode();

    game.window.clear();
    game.window.draw(backgroundSprite);

    for (int x = 0; x < boundaries.x; ++x) {
        for (int y = 0; y < boundaries.y; ++y) {
            game.window.draw(placedTileSprites[x][y]);

            int tileID = placedTileIDs[x][y];
            if (tileID >= 0) {
                Tile &tile = tiles[tileID];
                sf::ConvexShape collisionShape = tile.collisionShape;

                // Set visual properties for debugging
                if (debugMode)
                {
                    collisionShape.setFillColor(sf::Color(255, 0, 0, 100)); // Semi-transparent red
                    collisionShape.setOutlineColor(sf::Color::Red);
                    collisionShape.setOutlineThickness(1.0f);

                    // Apply the tile's transform
                    sf::Transform transform = placedTileSprites[x][y].getTransform();
                    game.window.draw(collisionShape, transform);
                }
            }
        }
    }

    for (int playerIDX = 0; playerIDX < networks; ++playerIDX) {
        Player &player = players[playerIDX];
        Car &car = player.car;
        car.render(game.window);

        if(debugMode){
            auto &rays = player.rays;
            auto &collisionMarkers = player.collisionMarkers;

            for (const auto& ray : rays) {
                game.window.draw(ray);
            }

            for (const auto& marker : collisionMarkers) {
                game.window.draw(marker);
            }
        }
    }
}

void AiGameState::update(Game &game) {
    std::cout << "Updating!\n";
    sf::Vector2i &boundaries = this->getBoundaries();
    auto &placedTileIDs = this->getPlacedTileIDs();
    auto &placedTileSprites = this->getPlacedTileSprites();
    auto &tiles = this->getTiles();

    performRaycasts(game);

    // Car Movement
    this->updateAI();

    // Set DebugTimer
    this->setDebugTimer(getDebugTimer() + game.dt);

    for (int playerIDX = 0; playerIDX < networks; ++playerIDX) {
        Player &player = players[playerIDX];
        Car &car = player.car;

        //std::cout << "pos: [" << player.car.getCurrentPosition().x << "/" << player.car.getCurrentPosition().y << "]\n";

        if(!player.isDead) {

            car.update(game.dt);

            auto &rayDistances = player.rayDistances;

            if (getDebugTimer() >= 1.0f) {
                // Print ray distances
                // std::cout << "[DEBUG] Ray lengths: ";
                for (size_t i = 0; i < rayDistances.size(); ++i) {
                    //     std::cout << "Ray " << i << ": " << rayDistances[i] << " ";
                }
                std::cout << "\n";

                // Reset the timer
                setDebugTimer(0.0f);
            }

            // Get the car's transformed points
            sf::Transform carTransform = car.getCarSprite().getTransform();
            sf::FloatRect carLocalBounds = car.getCarSprite().getLocalBounds();

            std::vector<sf::Vector2f> carPoints(4);
            carPoints[0] = carTransform.transformPoint(sf::Vector2f(carLocalBounds.left, carLocalBounds.top));
            carPoints[1] = carTransform.transformPoint(
                    sf::Vector2f(carLocalBounds.left + carLocalBounds.width, carLocalBounds.top));
            carPoints[2] = carTransform.transformPoint(sf::Vector2f(carLocalBounds.left + carLocalBounds.width,
                                                                    carLocalBounds.top + carLocalBounds.height));
            carPoints[3] = carTransform.transformPoint(
                    sf::Vector2f(carLocalBounds.left, carLocalBounds.top + carLocalBounds.height));

            bool allPointsOnRoad = true;

            for (const auto &point: carPoints) {
                bool pointOnRoad = false;

                // Determine which tile the point is over
                int tileSize = game.getTileSize();
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

                        Tile &tile = tiles[tileID];
                        const sf::ConvexShape &collisionShape = tile.collisionShape;

                        sf::Sprite &tileSprite = placedTileSprites[x][y];

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
                std::cout << " player should die?!\n";
                player.isDead = true;
                player.points -= 1000;
            }
        }
    }
}

void AiGameState::handleInput(Game &game) {
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
}

void AiGameState::updateAI() {
    auto &rayAngles = this->getRayAngles();
    int raySize = rayAngles.size();

    std::vector<float> inputData(raySize * networks, 0.0f);
    for (int i = 0; i < networks; ++i) {
        for (int r = 0; r < raySize; ++r) {
            inputData[i * raySize + r] = players[i].rayDistances[r];
        }
    }
    af::array inputAf(raySize, 1, networks, inputData.data());
    af::array outputAf = network.feed_forward(inputAf);
    int outputNeurons = (int)outputAf.dims()[0];
    std::vector<float> outputVec(outputNeurons * networks, 0.0f);
    outputAf.host(outputVec.data());


    for (int playerIDX = 0; playerIDX < networks; ++playerIDX) {
        Player &player = players[playerIDX];
        Car &car = player.car;
        auto &rayDistances = player.rayDistances;
        if(outputVec[playerIDX * outputNeurons + 0] > 0.0f){
            car.setAcceleration(car.getAccelerationConstant());
        }else{
            car.setAcceleration(-car.getAccelerationConstant());
        }

        if(outputVec[playerIDX * outputNeurons + 1] > 0.0f){
            car.setAngularAcceleration(-car.getAngularAccelerationConstant());
        }else{
            car.setAngularAcceleration(car.getAngularAccelerationConstant());
        }
    }
}
