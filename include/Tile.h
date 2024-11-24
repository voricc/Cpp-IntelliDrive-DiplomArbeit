//
// Created by Tobias on 15.10.2024.
//

#ifndef INTELLIDRIVE_TILE_H
#define INTELLIDRIVE_TILE_H

#include "SFML/Graphics.hpp"
#include "iostream"

class Tile {

private:
    sf::Texture texture;
    std::string texturePath;
    std::vector<sf::Vector2f> collisionPolygon;

public:
    Tile() = default;
    explicit Tile(std::string path);
    ~Tile() = default;

    sf::ConvexShape collisionShape;
    void initializeCollisionShape();

    [[nodiscard]] sf::Texture& getTexture() { return texture; }
    [[nodiscard]] const std::vector<sf::Vector2f>& getCollisionPolygon() const { return collisionPolygon; }
    [[nodiscard]] const std::string& getTexturePath() const { return texturePath; }

    void setTexturePath(const std::string& path);

    void addCollisionPoint(const sf::Vector2f& point);
    void removeCollisionPoint(const int idx);
    void deletePolygon();
};

#endif //INTELLIDRIVE_TILE_H
