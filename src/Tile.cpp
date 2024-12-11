#include "../include/Tile.h"

#include <cmath>
#define M_PI 3.141592653589793238462643383279502884197169399375105820974944

Tile::Tile(std::string path) {
    texturePath = path;
    if (!texture.loadFromFile(path)) {
        std::cerr << "Error loading texture from " << path << std::endl;
    }
    initializeCollisionShape();
}

void Tile::addCollisionPoint(const sf::Vector2f& point) {
    collisionPolygon.emplace_back(point);
}


void Tile::initializeCollisionShape() {
    collisionPolygon.clear();

    sf::Vector2u texSize = texture.getSize();
    float width = static_cast<float>(texSize.x);
    float height = static_cast<float>(texSize.y);

    if (texturePath.find("straight") != std::string::npos) {
        // Full rectangle for straight tiles
        collisionPolygon.push_back(sf::Vector2f(0, 0));
        collisionPolygon.push_back(sf::Vector2f(width, 0));
        collisionPolygon.push_back(sf::Vector2f(width, height));
        collisionPolygon.push_back(sf::Vector2f(0, height));
    } else if (texturePath.find("curve") != std::string::npos) {
        // Determine curve orientation
        bool isTop = texturePath.find("_t") != std::string::npos;
        bool isBottom = texturePath.find("_b") != std::string::npos;
        bool isLeft = texturePath.find("_l") != std::string::npos;
        bool isRight = texturePath.find("_r") != std::string::npos;

        int pointCount = 20;
        float radius = width; // Assuming square tiles

        std::vector<sf::Vector2f> arcPoints;

        if (isTop && isLeft) {
            // Curve in top-left corner (90 to 180 degrees)
            for (int i = 0; i <= pointCount; ++i) {
                float angle = (M_PI / 2) + (i / static_cast<float>(pointCount)) * (M_PI / 2); // 90 to 180 degrees
                float x = radius * cos(angle);
                float y = radius * sin(angle);
                arcPoints.push_back(sf::Vector2f(radius + x, radius + y));
            }
            // Construct the collision polygon
            collisionPolygon.push_back(sf::Vector2f(0, 0));
            collisionPolygon.push_back(sf::Vector2f(width, 0));
            collisionPolygon.insert(collisionPolygon.end(), arcPoints.begin(), arcPoints.end());
            collisionPolygon.push_back(sf::Vector2f(0, height));
        } else if (isTop && isRight) {
            // Curve in top-right corner (0 to 90 degrees)
            for (int i = 0; i <= pointCount; ++i) {
                float angle = (i / static_cast<float>(pointCount)) * (M_PI / 2); // 0 to 90 degrees
                float x = radius * cos(angle);
                float y = radius * sin(angle);
                arcPoints.push_back(sf::Vector2f(radius + x, radius - y));
            }
            collisionPolygon.push_back(sf::Vector2f(0, 0));
            collisionPolygon.insert(collisionPolygon.end(), arcPoints.begin(), arcPoints.end());
            collisionPolygon.push_back(sf::Vector2f(width, height));
            collisionPolygon.push_back(sf::Vector2f(0, height));
        } else if (isBottom && isRight) {
            // Curve in bottom-right corner (270 to 360 degrees)
            for (int i = 0; i <= pointCount; ++i) {
                float angle = (3 * M_PI / 2) + (i / static_cast<float>(pointCount)) * (M_PI / 2); // 270 to 360 degrees
                float x = radius * cos(angle);
                float y = radius * sin(angle);
                arcPoints.push_back(sf::Vector2f(radius + x, radius + y));
            }
            collisionPolygon.push_back(sf::Vector2f(0, 0));
            collisionPolygon.push_back(sf::Vector2f(width, 0));
            collisionPolygon.push_back(sf::Vector2f(width, height));
            collisionPolygon.insert(collisionPolygon.end(), arcPoints.begin(), arcPoints.end());
        } else if (isBottom && isLeft) {
            // Curve in bottom-left corner (180 to 270 degrees)
            for (int i = 0; i <= pointCount; ++i) {
                float angle = M_PI + (i / static_cast<float>(pointCount)) * (M_PI / 2); // 180 to 270 degrees
                float x = radius * cos(angle);
                float y = radius * sin(angle);
                arcPoints.push_back(sf::Vector2f(radius + x, radius + y));
            }
            collisionPolygon.insert(collisionPolygon.end(), arcPoints.begin(), arcPoints.end());
            collisionPolygon.push_back(sf::Vector2f(0, height));
            collisionPolygon.push_back(sf::Vector2f(0, 0));
            collisionPolygon.push_back(sf::Vector2f(width, 0));
        } else {
            // Default to full rectangle
            collisionPolygon.push_back(sf::Vector2f(0, 0));
            collisionPolygon.push_back(sf::Vector2f(width, 0));
            collisionPolygon.push_back(sf::Vector2f(width, height));
            collisionPolygon.push_back(sf::Vector2f(0, height));
        }
    } else {
        // Default to full rectangle
        collisionPolygon.push_back(sf::Vector2f(0, 0));
        collisionPolygon.push_back(sf::Vector2f(width, 0));
        collisionPolygon.push_back(sf::Vector2f(width, height));
        collisionPolygon.push_back(sf::Vector2f(0, height));
    }

    // Set points to collisionShape
    collisionShape.setPointCount(collisionPolygon.size());
    for (size_t i = 0; i < collisionPolygon.size(); ++i) {
        collisionShape.setPoint(i, collisionPolygon[i]);
    }
}
