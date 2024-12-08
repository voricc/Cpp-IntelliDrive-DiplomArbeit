//
// Created by Voric and tobisdev on 11/08/2024.
//

#include "../include/GameState.h"
#include "../include/SettingsManager.h"
#include "../include/PauseState.h"
#include "../include/DeathState.h"
#include "../include/Car.h"
#include "../include/Game.h"

#define M_PI 3.141592653589793238462643383279502884197169399375105820974944

bool debugMode = SettingsManager::getInstance().getDebugMode();

bool getLineIntersection(sf::Vector2f p0, sf::Vector2f p1,
                         sf::Vector2f p2, sf::Vector2f p3,
                         sf::Vector2f& intersectionPoint) {
    sf::Vector2f s1 = p1 - p0;
    sf::Vector2f s2 = p3 - p2;

    float s, t;
    float denominator = (-s2.x * s1.y + s1.x * s2.y);

    if (fabs(denominator) < 1e-6) {
        return false;
    }
    s = (-s1.y * (p0.x - p2.x) + s1.x * (p0.y - p2.y)) / denominator;
    t = ( s2.x * (p0.y - p2.y) - s2.y * (p0.x - p2.x)) / denominator;

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
        intersectionPoint = p0 + (t * s1);
        return true;
    }

    return false;
}

GameState::GameState(Game& game, const std::string& levelFile) : car(game.getCar()) {
    ResourceManager& resourceManager = ResourceManager::getInstance();
    std::cout << "[DEBUG] Initializing GameStatee\n";
    placedTileSprites.clear();
    placedTileIDs.clear();
    std::cout << "[DEBUG] Loading tiles from CSV\n";
    resourceManager.loadTilesFromCSV("resources/Tiles/Tiles.csv");
    tiles = resourceManager.getTiles();
    loadLevelFromCSV(levelFile, game);

    initializeCar();
    initialiazeRays();
    GameStateBackground(game);
}

void GameState::GameStateBackground(Game& game)
{
    ResourceManager &resourceManager = ResourceManager::getInstance();

    std::cout << "[DEBUG] Setting up GameState background\n";

    if (!resourceManager.textureExists("background5")) {
        std::cout << "[DEBUG] Background texture not found. Loading texture 'background5'\n";
        resourceManager.loadTexture("background5", "resources/Backgrounds/background5.png");
    } else {
        std::cout << "[DEBUG] Background texture 'background5' already loaded\n";
    }

    sf::Texture& backgroundTexture = resourceManager.getTexture("background5");
    if (backgroundTexture.getSize().x == 0 || backgroundTexture.getSize().y == 0) {
        std::cerr << "[DEBUG] Background texture is invalid (size is zero)\n";
    } else {
        std::cout << "[DEBUG] Background texture loaded successfully\n";
    }
    backgroundSprite.setTexture(backgroundTexture);

    sf::Vector2u windowSize = game.window.getSize();
    sf::Vector2u textureSize = backgroundTexture.getSize();
    backgroundSprite.setScale(
        static_cast<float>(windowSize.x) / textureSize.x,
        static_cast<float>(windowSize.y) / textureSize.y
    );
}

void GameState::handleInput(Game& game) {
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
    car.handleInput();
}

void GameState::loadLevelFromCSV(const std::string& filename, Game &game) {
    std::cout << "[DEBUG] Level loading from file: " << filename << "\n";

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[DEBUG] Failed to open level file: " << filename << "\n";
        return;
    }
    std::string line;
    int idx = 0;

    while (std::getline(file, line)) {
        std::stringstream ss(line);

        if (idx == 0) {
            std::string xStr, yStr;
            if (std::getline(ss, xStr, ',') && std::getline(ss, yStr)) {
                boundaries.x = std::stoi(xStr);
                boundaries.y = std::stoi(yStr);
                std::cout << "[DEBUG] Level boundaries: x=" << boundaries.x << ", y=" << boundaries.y << "\n";

                placedTileIDs.resize(boundaries.x, std::vector<int>(boundaries.y, -1));
                placedTileSprites.resize(boundaries.x, std::vector<sf::Sprite>(boundaries.y));
            }
        } else {
            if (line.substr(0, 11) == "SPAWN_POINT") {
                std::string label;
                std::getline(ss, label, ',');
                std::string posXStr, posYStr, dirXStr, dirYStr;
                if (std::getline(ss, posXStr, ',') && std::getline(ss, posYStr, ',') &&
                    std::getline(ss, dirXStr, ',') && std::getline(ss, dirYStr)) {
                    spawnPointPosition.x = std::stof(posXStr);
                    spawnPointPosition.y = std::stof(posYStr);
                    spawnPointDirection.x = std::stof(dirXStr);
                    spawnPointDirection.y = std::stof(dirYStr);
                    hasSpawnPoint = true;
                    std::cout << "[DEBUG] Spawn point loaded: position (" << spawnPointPosition.x << ", " << spawnPointPosition.y << "), direction (" << spawnPointDirection.x << ", " << spawnPointDirection.y << ")\n";
                }
            } else {
                std::string xStr, yStr, textureStr;
                if (std::getline(ss, xStr, ',') && std::getline(ss, yStr, ',') && std::getline(ss, textureStr)) {
                    int x = std::stoi(xStr);
                    int y = std::stoi(yStr);
                    int texture = std::stoi(textureStr);

                    if (x < 0 || x >= boundaries.x || y < 0 || y >= boundaries.y){
                        std::cerr << "[DEBUG] The file seems to be corrupt! Tile position out of bounds at idx " << idx << "\n";
                        return;
                    }

                    if (texture < 0 || texture >= tiles.size()) {
                        continue;
                    }

                    sf::Sprite s;
                    s.setTexture(tiles[texture].getTexture());
                    s.setScale(game.getTileSize() / s.getLocalBounds().height, game.getTileSize() / s.getLocalBounds().height);
                    s.setPosition(game.getTileSize() * x, game.getTileSize() * y);

                    placedTileSprites[x][y] = s;
                    placedTileIDs[x][y] = texture;
                }
            }
        }
        idx++;
    }
    std::cout << "[DEBUG] Level loaded successfully\n";
}

void GameState::initialiazeRays() {
    std::cout << "[DEBUG] Initializing rays\n";
    rayDistances.resize(5, 0.0f);
    rays.resize(5);
    collisionMarkers.reserve(5);
}

bool GameState::isPointInPolygon(const sf::Vector2f& point, const sf::ConvexShape& polygon) {
    int i, j, nvert = polygon.getPointCount();
    bool c = false;
    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        sf::Vector2f pi = polygon.getTransform().transformPoint(polygon.getPoint(i));
        sf::Vector2f pj = polygon.getTransform().transformPoint(polygon.getPoint(j));
        if ( ((pi.y > point.y) != (pj.y > point.y)) &&
            (point.x < (pj.x - pi.x) * (point.y - pi.y) / (pj.y - pi.y) + pi.x) )
            c = !c;
    }
    return c;
}

void GameState::performRaycasts(Game& game) {
    float rotation_angle = car.getRotationAngle();
    sf::Vector2f carPosition = car.getCurrentPosition();

    // Define the angles for the rays relative to the car's rotation
    std::vector<float> rayAngles = {
        rotation_angle - 90.0f,
        rotation_angle + 90.0f,
        rotation_angle,
        rotation_angle - 45.0f,
        rotation_angle + 45.0f
    };

    // Normalize angles between 0 and 360 degrees
    for (auto& angle : rayAngles) {
        angle = fmod(angle + 360.0f, 360.0f);
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
        if (debugMode == true) {
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


void GameState::update(Game& game) {
    car.update(game.dt);
    performRaycasts(game);

    std::cout << "[DEBUG] Debug Mode: " << debugMode << "\n";

    debugTimer += game.dt;
    if (debugTimer >= 1.0f) {
        // Print ray distances
       // std::cout << "[DEBUG] Ray lengths: ";
        for (size_t i = 0; i < rayDistances.size(); ++i) {
       //     std::cout << "Ray " << i << ": " << rayDistances[i] << " ";
        }
        std::cout << "\n";

        // Reset the timer
        debugTimer = 0.0f;
    }

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

void GameState::render(Game& game) {
    game.window.clear();
    game.window.draw(backgroundSprite);
    for (int x = 0; x < boundaries.x; ++x) {
        for (int y = 0; y < boundaries.y; ++y) {
            game.window.draw(placedTileSprites[x][y]);

            int tileID = placedTileIDs[x][y];
            if (tileID >= 0) {
                Tile& tile = tiles[tileID];
                sf::ConvexShape collisionShape = tile.collisionShape;

            // Set visual properties for debugging
            if (debugMode == true)
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

    car.render(game.window);

    for (const auto& ray : rays) {
        game.window.draw(ray);
    }

    for (const auto& marker : collisionMarkers) {
        game.window.draw(marker);
    }
}

void GameState::initializeCar() {
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

bool GameState::isPauseKeyPressed(const sf::Event& event) const {
    return (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P);
}

sf::RectangleShape GameState::createRoad(Game& game) const {
    sf::RectangleShape road(sf::Vector2f(game.window.getSize().x - 20, game.window.getSize().y - 20));
    road.setPosition(10, 10);
    road.setFillColor(sf::Color{80, 80, 80});
    return road;
}

bool GameState::isPointInPolygon(const sf::Vector2f& point, const sf::ConvexShape& polygon, const sf::Transform& transform) {
    int i, j, nvert = polygon.getPointCount();
    bool c = false;
    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        sf::Vector2f pi = transform.transformPoint(polygon.getPoint(i));
        sf::Vector2f pj = transform.transformPoint(polygon.getPoint(j));
        if (((pi.y > point.y) != (pj.y > point.y)) &&
            (point.x < (pj.x - pi.x) * (point.y - pi.y) / (pj.y - pi.y) + pi.x))
            c = !c;
    }
    return c;
}
