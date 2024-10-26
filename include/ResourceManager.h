//
// Created by Voric on 11/08/2024.
//

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Tile.h"

class ResourceManager {
public:
    static ResourceManager& getInstance();

    void loadFont(const std::string& name, const std::string& filename);
    [[ nodiscard ]] sf::Font& getFont(const std::string& name);

    bool loadTexture(const std::string &name, const std::string &filename);
    void setTexture(const std::string &name, const std::string &filename);

    [[ nodiscard ]] sf::Texture& getTexture(const std::string& name);

    void setTiles(std::vector<Tile> &t) { tiles = t; };
    [[ nodiscard ]] std::vector<Tile> &getTiles() { return tiles; };

    void saveTilesToCSV(const std::string& filename);
    void loadTilesFromCSV(const std::string& filename);
    bool textureExists(const std::string& textureKey) const;

    void setTile(int i, Tile &t) { tiles[i] = t; };
    [[ nodiscard ]] Tile &getTile(int i) { return tiles[i]; };

private:
    ResourceManager() = default;

    std::unordered_map<std::string, sf::Font> fonts;
    std::unordered_map<std::string, sf::Texture> textures;
    std::vector<Tile> tiles;
};

#endif // RESOURCEMANAGER_H
