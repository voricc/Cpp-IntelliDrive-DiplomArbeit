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
    static void loadAllKnownResources();

    static void loadFont(const std::string& name, const std::string& filename);
    [[ nodiscard ]] static sf::Font& getFont(const std::string& name);

    static bool loadTexture(const std::string &name, const std::string &filename);
    static void setTexture(const std::string &name, const std::string &filename);

    [[ nodiscard ]] static sf::Texture& getTexture(const std::string& name);

    [[ nodiscard ]] static std::vector<Tile> &getTiles() { return tiles; };

    static void loadTilesFromCSV(const std::string& filename);
    static bool textureExists(const std::string& textureKey);

    static void setTile(int i, Tile &t) { tiles[i] = t; };
    [[ nodiscard ]] static Tile &getTile(int i) { return tiles[i]; };

private:
    static std::unordered_map<std::string, sf::Font> fonts;
    static std::unordered_map<std::string, sf::Texture> textures;
    static std::vector<Tile> tiles;
};

#endif // RESOURCEMANAGER_H
