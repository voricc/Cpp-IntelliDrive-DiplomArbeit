//
// Created by Voric on 11/08/2024.
//

#include "../include/ResourceManager.h"

// Definition der statischen Variablen
std::unordered_map<std::string, sf::Font> ResourceManager::fonts;
std::unordered_map<std::string, sf::Texture> ResourceManager::textures;
std::vector<Tile> ResourceManager::tiles;


void ResourceManager::loadAllKnownResources() {
    std::cout << "Loading resources...\n";

    // Load all known fonts
    loadFont("Rubik-Regular", VariableManager::getFontRubikRegular());
    loadFont("UpheavalPRO", VariableManager::getFontUpheavalPro());
    loadFont("MenuTitle", VariableManager::getFontMenuTitle());

    // Load all tiles
    loadTilesFromCSV(VariableManager::getPathToTileConfig());

    // Load all known textures
    loadTexture("CarChoosingArrowLeft", VariableManager::getImageCarChoosingArrowLeft());
    loadTexture("CarChoosingArrowRight", VariableManager::getImageCarChoosingArrowRight());
    loadTexture("GameBackground", VariableManager::getImageGameBackground());
    loadTexture("BackgroundCarChoosing", VariableManager::getImageBackgroundCarChoosing());

    // Load multiple background images (e.g., background1.png, background2.png, etc.)
    for (int i = 1; i <= 5; ++i) {
        std::string backgroundName = "Background" + std::to_string(i);
        std::string backgroundPath = VariableManager::getPathToBackgrounds() + std::to_string(i) + ".png";
        loadTexture(backgroundName, backgroundPath);
    }

    std::cout << "All known resources loaded successfully." << std::endl;
}

void ResourceManager::loadFont(const std::string& name, const std::string& filename) {
    sf::Font font;
    if (font.loadFromFile(filename)) {
        fonts[name] = std::move(font);
    } else {
        std::cerr << "Error loading font: " << filename << std::endl;
    }
}

sf::Font& ResourceManager::getFont(const std::string& name) {
    return fonts.at(name);
}

bool ResourceManager::loadTexture(const std::string &name, const std::string &filename) {
    sf::Texture texture;
    if (texture.loadFromFile(filename)) {
        textures[name] = std::move(texture);
        return true;
    } else {
        std::cerr << "Error loading texture: " << filename << std::endl;
        return false;
    }
}

void ResourceManager::setTexture(const std::string& name, const std::string& filename) {
    sf::Texture texture;
    if (texture.loadFromFile(filename)) {
        textures[name] = std::move(texture);
    } else {
        std::cerr << "Error loading texture: " << filename << std::endl;
    }
}

sf::Texture& ResourceManager::getTexture(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    } else {
        throw std::runtime_error("Texture not found: " + name);
    }
}

void ResourceManager::loadTilesFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    tiles.clear();
    if (!file.is_open()) {
        std::cerr << "Error opening file for loading" << std::endl;
        return;
    }

    tiles.clear();

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string texturePath;
        std::getline(ss, texturePath, ',');

        Tile tile(texturePath);

        size_t polygonSize;
        ss >> polygonSize;
        ss.ignore(1, ',');

        for (size_t i = 0; i < polygonSize; ++i) {
            float x, y;
            ss >> x;
            ss.ignore(1, ',');
            ss >> y;
            if (i < polygonSize - 1) {
                ss.ignore(1, ',');
            }

            tile.addCollisionPoint(sf::Vector2f(x, y));
        }

        tiles.push_back(tile);
    }

    file.close();
    std::cout << "Tiles loaded from " << filename << std::endl;
    return;
}

bool ResourceManager::textureExists(const std::string& textureKey) {
    return textures.find(textureKey) != textures.end();
}
