//
// Created by Tobias on 17.12.2024.
//

#include "GameStateParent.h"


bool GameStateParent::getLineIntersection(sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3,
                                          sf::Vector2f &intersectionPoint) {
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

void GameStateParent::loadLevelFromCSV(const std::string &filename, Game &game) {
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

void GameStateParent::loadBackground(Game &game) {
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

GameStateParent::GameStateParent(Game &game, const std::string &levelFile) {
    ResourceManager& resourceManager = ResourceManager::getInstance();
    std::cout << "[DEBUG] Initializing GameStatee\n";
    placedTileSprites.clear();
    placedTileIDs.clear();
    std::cout << "[DEBUG] Loading tiles from CSV\n";
    resourceManager.loadTilesFromCSV("resources/Tiles/Tiles.csv");
    tiles = resourceManager.getTiles();
    loadLevelFromCSV(levelFile, game);
    loadBackground(game);
}

bool GameStateParent::isPointInPolygon(const sf::Vector2f &point, const sf::ConvexShape &polygon) {
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

bool GameStateParent::isPointInPolygon(const sf::Vector2f& point, const sf::ConvexShape& polygon, const sf::Transform& transform) {
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

bool GameStateParent::isPauseKeyPressed(const sf::Event &event) const {
    return (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P);
}
