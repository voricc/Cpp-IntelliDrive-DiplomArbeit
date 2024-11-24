//
// Created by Tobias on 15.10.2024.
//

#include "../include/Tile.h"

#include <cmath>

Tile::Tile(std::string path) {
    texturePath = path;
    if (!texture.loadFromFile(path)) {
        std::cerr << "Error loading texture from " << path << std::endl;
    }
    initializeCollisionShape();
}

void Tile::addCollisionPoint(const sf::Vector2f &point) {
    collisionPolygon.emplace_back(point);
}

void Tile::setTexturePath(const std::string &path) {
    texturePath = path;
    if (!texture.loadFromFile(path)) {
        std::cerr << "Error loading texture from " << path << std::endl;
    }
    initializeCollisionShape();
}

void Tile::removeCollisionPoint(const int idx) {
    if(idx >= 0 && idx < collisionPolygon.size()){
        collisionPolygon.erase(collisionPolygon.begin() + idx);
    }
}

void Tile::deletePolygon() {
    collisionPolygon.clear();
}

void Tile::initializeCollisionShape() {
    collisionPolygon.clear();

    if (texturePath.find("straight") != std::string::npos) {
        // Full rectangle for straight tiles
        collisionPolygon.push_back(sf::Vector2f(0, 0));
        collisionPolygon.push_back(sf::Vector2f(texture.getSize().x, 0));
        collisionPolygon.push_back(sf::Vector2f(texture.getSize().x, texture.getSize().y));
        collisionPolygon.push_back(sf::Vector2f(0, texture.getSize().y));
    } else if (texturePath.find("curve") != std::string::npos) {
        // Define collision shape for curved tiles (quarter circle)
        int pointCount = 20;
        float radius = texture.getSize().x;
        for (int i = 0; i <= pointCount; ++i) {
            float angle = (i / static_cast<float>(pointCount)) * (M_PI / 2); // 90 degrees
            float x = radius * cos(angle);
            float y = radius * sin(angle);
            collisionPolygon.push_back(sf::Vector2f(radius - x, radius - y));
        }
        // Close the shape to the center point
        collisionPolygon.push_back(sf::Vector2f(radius, radius));
    } else {
        // Default to full rectangle
        collisionPolygon.push_back(sf::Vector2f(0, 0));
        collisionPolygon.push_back(sf::Vector2f(texture.getSize().x, 0));
        collisionPolygon.push_back(sf::Vector2f(texture.getSize().x, texture.getSize().y));
        collisionPolygon.push_back(sf::Vector2f(0, texture.getSize().y));
    }

    collisionShape.setPointCount(collisionPolygon.size());
    for (size_t i = 0; i < collisionPolygon.size(); ++i) {
        collisionShape.setPoint(i, collisionPolygon[i]);
    }
}
