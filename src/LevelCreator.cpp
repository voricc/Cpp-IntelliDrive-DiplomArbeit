//
// Created by Devrim on 30.09.2024.
//
// LevelCreator.cpp

#include "../include/LevelCreator.h"
#include "../include/ResourceManager.h"
#include "../include/MenuState.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm> // For std::find
#include <numeric>   // For std::iota
#include <sstream>   // For std::stringstream
#include <filesystem> // For directory traversal
namespace fs = std::filesystem;

LevelCreator::LevelCreator(Game& game) : GameState(game), showExplanation(true), currentState(EditorState::Edit) {
    initializeResources(game);
    createButtons(game);

    boundaries = {static_cast<int>(game.window.getSize().x / game.getTileSize()), static_cast<int>(game.window.getSize().y / game.getTileSize()) + 1};

    placedTileIDs.resize(boundaries.x, std::vector<int>(boundaries.y, -1));
    placedTileSprites.resize(boundaries.x, std::vector<sf::Sprite>(boundaries.y));
}

void LevelCreator::initializeResources(Game& game) {
    ResourceManager& resourceManager = ResourceManager::getInstance();

    resourceManager.loadTilesFromCSV("resources/Tiles/Tiles.csv");
    tiles = resourceManager.getTiles();

    resourceManager.loadFont("Rubik-Regular", "resources/Fonts/Rubik-Regular.ttf");
    font = resourceManager.getFont("Rubik-Regular");

    resourceManager.loadTexture("background5", "resources/Backgrounds/background5.png");
    sf::Texture& backgroundTexture = resourceManager.getTexture("background5");
    backgroundSprite.setTexture(backgroundTexture);

    sf::Vector2u windowSize = game.window.getSize();
    sf::Vector2u textureSize = backgroundTexture.getSize();
    backgroundSprite.setScale(
        static_cast<float>(windowSize.x) / textureSize.x,
        static_cast<float>(windowSize.y) / textureSize.y
    );

    // Initialize helpText
    helpText.setFont(font);
    helpText.setCharacterSize(20);
    helpText.setFillColor(sf::Color::White);
}

void LevelCreator::createButtons(Game& game) {
    saveButton.setSize({100.0f, 30.0f});
    saveButton.setPosition(game.window.getSize().x - 220.0f, game.window.getSize().y - 40.0f);
    saveButton.setFillColor(sf::Color::Green);

    saveButtonText.setFont(font);
    saveButtonText.setString("Save");
    saveButtonText.setCharacterSize(20);
    saveButtonText.setFillColor(sf::Color::White);
    saveButtonText.setPosition(saveButton.getPosition().x + 25.0f, saveButton.getPosition().y + 5.0f);

    exitButton.setSize({100.0f, 30.0f});
    exitButton.setPosition(game.window.getSize().x - 110.0f, game.window.getSize().y - 40.0f);
    exitButton.setFillColor(sf::Color::Red);

    exitButtonText.setFont(font);
    exitButtonText.setString("Exit");
    exitButtonText.setCharacterSize(20);
    exitButtonText.setFillColor(sf::Color::White);
    exitButtonText.setPosition(exitButton.getPosition().x + 30.0f, exitButton.getPosition().y + 5.0f);

    // Load Button (Moved to bottom left)
    loadButton.setSize({100.0f, 30.0f});
    loadButton.setPosition(10.0f, game.window.getSize().y - 40.0f);
    loadButton.setFillColor(sf::Color::Blue);

    loadButtonText.setFont(font);
    loadButtonText.setString("Load");
    loadButtonText.setCharacterSize(20);
    loadButtonText.setFillColor(sf::Color::White);
    loadButtonText.setPosition(loadButton.getPosition().x + 25.0f, loadButton.getPosition().y + 5.0f);
}

void LevelCreator::handleInput(Game& game) {
    sf::Event event;
    while (game.window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            game.window.close();
        }

        if (showExplanation) {
            handleExplanationInput(event);
            continue;
        }

        if (inputActive) {
            handleTextInput(event, game);
        } else {
            handleKeyboardInput(event, game);

            switch (currentState) {
                case EditorState::Edit:
                    handleEditInput(event, game);
                    break;
                case EditorState::View:
                    handleViewInput(event, game);
                    break;
                case EditorState::Buttons:
                    handleButtonsInput(event, game);
                    break;
            }
        }
    }
}

void LevelCreator::handleExplanationInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed) {
        showExplanation = false;
    }
}

void LevelCreator::handleTextInput(const sf::Event& event, Game& game) {
    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode < 128) {
            if (event.text.unicode == '\b' && !inputFileName.empty()) {
                inputFileName.pop_back();
            } else if (event.text.unicode != '\b' && event.text.unicode != '\r' && event.text.unicode != '\n') {
                inputFileName += static_cast<char>(event.text.unicode);
            }
        }
    }

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter) {
            if (isSaving) {
                saveLevelToCSV(inputFileName + ".csv");
                clearDrawing(game);
            } else {
                loadLevelFromCSV(inputFileName + ".csv", game);
            }
            inputActive = false;
        } else if (event.key.code == sf::Keyboard::Escape) {
            inputActive = false;
        }
    }
}

void LevelCreator::handleKeyboardInput(const sf::Event& event, Game& game) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Num1) {
            currentState = EditorState::Buttons;
        } else if (event.key.code == sf::Keyboard::Num2) {
            currentState = EditorState::View;
        } else if (event.key.code == sf::Keyboard::Num3) {
            currentState = EditorState::Edit;
        }
        if (event.key.code == sf::Keyboard::S) {
            inputActive = true;
            inputFileName.clear();
            isSaving = true;
        }
        if (event.key.code == sf::Keyboard::Escape) {
            game.changeState(std::make_shared<MenuState>());
        }
    }
}

void LevelCreator::handleEditInput(const sf::Event& event, Game& game) {
    sf::Vector2f mousePos = game.window.mapPixelToCoords(sf::Mouse::getPosition(game.window));

    if (event.type == sf::Event::MouseWheelScrolled) {
        if (totalTiles == 0) return; // No tiles to scroll through

        if (event.mouseWheelScroll.delta > 0) {
            // Inverted scrolling: scrolling up moves left
            if (selectingFavorites) {
                selectedFavoriteIndex = (selectedFavoriteIndex - 1 + totalTiles) % totalTiles;
                selectedTile = tileIndices[selectedFavoriteIndex];
            } else {
                selectedTile = (selectedTile - 1 + tiles.size()) % tiles.size();
            }
        } else if (event.mouseWheelScroll.delta < 0) {
            // Inverted scrolling: scrolling down moves right
            if (selectingFavorites) {
                selectedFavoriteIndex = (selectedFavoriteIndex + 1) % totalTiles;
                selectedTile = tileIndices[selectedFavoriteIndex];
            } else {
                selectedTile = (selectedTile + 1) % tiles.size();
            }
        }
    }

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::F) {
            auto it = std::find(favoriteTiles.begin(), favoriteTiles.end(), selectedTile);
            if (it == favoriteTiles.end()) {
                favoriteTiles.push_back(selectedTile);
            } else {
                favoriteTiles.erase(it);
            }
        }
        if (event.key.code == sf::Keyboard::Tab) {
            selectingFavorites = !selectingFavorites;
            if (selectingFavorites && !favoriteTiles.empty()) {
                selectedFavoriteIndex = 0;
                selectedTile = favoriteTiles[selectedFavoriteIndex];
            }
        }
        if (event.key.code == sf::Keyboard::C) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(game.window);
            sf::Vector2i grid(static_cast<int>(mousePos.x / game.getTileSize()), static_cast<int>(mousePos.y / game.getTileSize()));

            if (grid.x >= 0 && grid.x < boundaries.x && grid.y >= 0 && grid.y < boundaries.y) {
                int tileID = placedTileIDs[grid.x][grid.y];
                if (tileID != -1) {
                    selectedTile = tileID;
                }
            }
        }
        if (event.key.code == sf::Keyboard::E) {
            tileEditMode = (tileEditMode == TileEditMode::Place) ? TileEditMode::Erase : TileEditMode::Place;
        }
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            if (saveButton.getGlobalBounds().contains(mousePos)) {
                inputActive = true;
                inputFileName.clear();
                isSaving = true;
            } else if (exitButton.getGlobalBounds().contains(mousePos)) {
                clearDrawing(game);
            } else if (loadButton.getGlobalBounds().contains(mousePos)) {
                inputActive = true;
                inputFileName.clear();
                isSaving = false;
            } else {
                // Removed changing selected tile when clicking on showing tiles
                mouseDown = true;
            }
        } else if (event.mouseButton.button == sf::Mouse::Right) {
            // Toggle favorite status of the tile in the selection UI
            bool toggledFavorite = false;
            for (int i = 0; i < tileSelectionSprites.size(); ++i) {
                if (tileSelectionSprites[i].getGlobalBounds().contains(mousePos)) {
                    int index = (selectedIndex - tileSelectionRange + i + totalTiles) % totalTiles;
                    int tileID = tileIndices[index];
                    auto it = std::find(favoriteTiles.begin(), favoriteTiles.end(), tileID);
                    if (it == favoriteTiles.end()) {
                        favoriteTiles.push_back(tileID);
                    } else {
                        favoriteTiles.erase(it);
                    }
                    toggledFavorite = true;
                    break;
                }
            }
            if (!toggledFavorite) {
                rightMouseDown = true;
            }
        }
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            mouseDown = false;
        } else if (event.mouseButton.button == sf::Mouse::Right) {
            rightMouseDown = false;
        }
    }

    if (mouseDown) {
        addTileAtMouse(game);
    }

    if (rightMouseDown) {
        removeTileAtMouse(game);
    }
}

void LevelCreator::handleViewInput(const sf::Event& event, Game& game) {
    // Implement if needed
}

void LevelCreator::handleButtonsInput(const sf::Event& event, Game& game) {
    // Implement if needed
}

void LevelCreator::update(Game& game) {
    if (showExplanation || inputActive) {
        return;
    }

    // Update the help text
    std::string modeString;
    switch (currentState) {
        case EditorState::Edit:
            modeString = "Mode: Edit";
            if (selectingFavorites) {
                modeString += " (Favorites)";
            } else {
                modeString += " (All Tiles)";
            }
            break;
        case EditorState::View:
            modeString = "Mode: View";
            break;
        case EditorState::Buttons:
            modeString = "Mode: Buttons";
            break;
    }
    helpText.setString(modeString);
    helpText.setPosition(10.0f, 10.0f);

    switch (currentState) {
        case EditorState::Edit:
            updateEditState(game);
            break;
        case EditorState::View:
            updateViewState(game);
            break;
        case EditorState::Buttons:
            updateButtonsState(game);
            break;
    }
}

void LevelCreator::updateEditState(Game& game) {
    updateTileSelectionUI(game);
    updatePreviewTile(game);
}

void LevelCreator::updateViewState(Game& game) {
    // Implement if needed
}

void LevelCreator::updateButtonsState(Game& game) {
    // Implement if needed
}

void LevelCreator::updateTileSelectionUI(Game& game) {
    int displaySize = 7; // Number of tiles to display
    tileSelectionRange = displaySize / 2;

    tileSelectionSprites.clear();
    tileSelectionBorders.clear(); // Clear borders

    tileIndices.clear();
    if (selectingFavorites) {
        tileIndices = favoriteTiles;
    } else {
        tileIndices.resize(tiles.size());
        std::iota(tileIndices.begin(), tileIndices.end(), 0);
    }

    totalTiles = tileIndices.size();

    if (totalTiles == 0) return; // No tiles to display

    if (selectingFavorites) {
        selectedIndex = selectedFavoriteIndex;
    } else {
        selectedIndex = selectedTile;
    }

    for (int i = -tileSelectionRange; i <= tileSelectionRange; ++i) {
        int index = (selectedIndex + i + totalTiles) % totalTiles;
        int tileID = tileIndices[index];
        sf::Sprite sprite;
        sprite.setTexture(tiles[tileID].getTexture());

        // Increase scaleFactor to make tiles bigger
        float scaleFactor = 1.2f; // Increased from 0.8f to 1.2f
        float scale = game.getTileSize() / sprite.getLocalBounds().height * scaleFactor;
        sprite.setScale(scale, scale);

        // Adjust the spacing to prevent overlap
        float spacing = game.getTileSize() * scaleFactor * 1.1f; // Adjusted spacing factor
        float xPos = game.window.getSize().x / 2 + i * spacing;
        float yPos = game.window.getSize().y - game.getTileSize() * scaleFactor * 1.2f; // Adjust yPos as needed
        sprite.setPosition(xPos, yPos);

        // Set transparency (optional)
        int maxAlpha = 255;
        int minAlpha = 100;
        int alphaRange = maxAlpha - minAlpha;
        int alpha = maxAlpha - (std::abs(i) * (alphaRange / tileSelectionRange));
        if (alpha < minAlpha) alpha = minAlpha;
        sprite.setColor(sf::Color(255, 255, 255, alpha));

        tileSelectionSprites.push_back(sprite);

        // Create a border rectangle around the sprite
        sf::FloatRect spriteBounds = sprite.getGlobalBounds();
        sf::RectangleShape border(sf::Vector2f(spriteBounds.width, spriteBounds.height));
        border.setPosition(spriteBounds.left, spriteBounds.top);
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineColor(sf::Color{40,40,40});
        border.setOutlineThickness(2.0f);
        // No need to set origin here
        //tileSelectionBorders.push_back(border);
    }
}


void LevelCreator::updatePreviewTile(Game& game) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(game.window);
    float snappedX = static_cast<int>(mousePos.x / game.getTileSize()) * game.getTileSize();
    float snappedY = static_cast<int>(mousePos.y / game.getTileSize()) * game.getTileSize();

    if (selectedTile != -1) {
        previewTile.setTexture(tiles[selectedTile].getTexture());
        previewTile.setScale(game.getTileSize() / previewTile.getLocalBounds().height, game.getTileSize() / previewTile.getLocalBounds().height);
        previewTile.setPosition(snappedX, snappedY);
        previewTile.setColor(sf::Color(255, 255, 255, 128));
    }
}

void LevelCreator::render(Game& game) {
    game.window.clear();
    game.window.draw(backgroundSprite);

    switch (currentState) {
        case EditorState::Edit:
            renderEditState(game);
            break;
        case EditorState::View:
            renderViewState(game);
            break;
        case EditorState::Buttons:
            renderButtonsState(game);
            break;
    }

    // Draw the help text with background
    sf::RectangleShape helpTextBackground(sf::Vector2f(helpText.getLocalBounds().width + 10, helpText.getLocalBounds().height + 10));
    helpTextBackground.setPosition(helpText.getPosition().x - 5, helpText.getPosition().y - 5);
    helpTextBackground.setFillColor(sf::Color(0, 0, 0, 150));
    game.window.draw(helpTextBackground);
    game.window.draw(helpText);

    if (showExplanation) {
        drawExplanationScreen(game);
    }

    if (inputActive) {
        drawInputBox(game);
    }

    drawButtons(game);
}

void LevelCreator::renderEditState(Game& game) {
    drawPlacedTiles(game);
    updatePreviewTile(game);

    // Compute bounding rectangle of tile selection sprites
    if (!tileSelectionSprites.empty()) {
        sf::FloatRect boundingRect = tileSelectionSprites[0].getGlobalBounds();
        for (const auto& sprite : tileSelectionSprites) {
            sf::FloatRect spriteBounds = sprite.getGlobalBounds();

            float left = std::min(boundingRect.left, spriteBounds.left);
            float top = std::min(boundingRect.top, spriteBounds.top);
            float right = std::max(boundingRect.left + boundingRect.width, spriteBounds.left + spriteBounds.width);
            float bottom = std::max(boundingRect.top + boundingRect.height, spriteBounds.top + spriteBounds.height);

            boundingRect.left = left;
            boundingRect.top = top;
            boundingRect.width = right - left;
            boundingRect.height = bottom - top;
        }

        // Create background rectangle similar to helpText
        sf::RectangleShape tileSelectionBackground(sf::Vector2f(boundingRect.width + 20, boundingRect.height + 20));
        tileSelectionBackground.setPosition(boundingRect.left - 10, boundingRect.top - 10);
        tileSelectionBackground.setFillColor(sf::Color(0, 0, 0, 150));
        game.window.draw(tileSelectionBackground);
    }

    // Draw the Tile Selection UI
    for (const auto& sprite : tileSelectionSprites) {
        game.window.draw(sprite);
    }
    // Draw the borders
    for (const auto& border : tileSelectionBorders) {
        game.window.draw(border);
    }

    // Draw the Preview Tile
    if (selectedTile != -1) {
        game.window.draw(previewTile);
    }
}


void LevelCreator::renderViewState(Game& game) {
    drawPlacedTiles(game);
    // Implement additional rendering if needed
}

void LevelCreator::renderButtonsState(Game& game) {
    drawPlacedTiles(game);
    // Implement additional rendering if needed
}

void LevelCreator::addTileAtMouse(Game& game) {
    if (selectedTile == -1) {
        return; // No tile to place
    }

    sf::Vector2i mousePos = sf::Mouse::getPosition(game.window);
    sf::Vector2i grid(static_cast<int>(mousePos.x / game.getTileSize()), static_cast<int>(mousePos.y / game.getTileSize()));

    if (grid.x < 0 || grid.x >= boundaries.x || grid.y < 0 || grid.y >= boundaries.y) {
        return;
    }

    float snappedX = grid.x * game.getTileSize();
    float snappedY = grid.y * game.getTileSize();

    sf::Sprite s;
    s.setTexture(tiles[selectedTile].getTexture());
    s.setScale(game.getTileSize() / s.getLocalBounds().height, game.getTileSize() / s.getLocalBounds().height);
    s.setPosition(snappedX, snappedY);

    placedTileIDs[grid.x][grid.y] = selectedTile;
    placedTileSprites[grid.x][grid.y] = s;
}

void LevelCreator::removeTileAtMouse(Game& game) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(game.window);
    sf::Vector2i grid(static_cast<int>(mousePos.x / game.getTileSize()), static_cast<int>(mousePos.y / game.getTileSize()));

    if (grid.x < 0 || grid.x >= boundaries.x || grid.y < 0 || grid.y >= boundaries.y) {
        return;
    }

    placedTileIDs[grid.x][grid.y] = -1;
    placedTileSprites[grid.x][grid.y] = {};
}

void LevelCreator::drawPlacedTiles(Game& game) {
    for (const auto& tileX : placedTileSprites) {
        for (const auto& tileY : tileX) {
            game.window.draw(tileY);
        }
    }
}

void LevelCreator::drawButtons(Game& game) {
    game.window.draw(saveButton);
    game.window.draw(saveButtonText);
    game.window.draw(exitButton);
    game.window.draw(exitButtonText);
    game.window.draw(loadButton);
    game.window.draw(loadButtonText);
}

void LevelCreator::drawExplanationScreen(Game& game) {
    sf::RectangleShape overlay(sf::Vector2f(game.window.getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    game.window.draw(overlay);

    sf::Text explanationText;
    explanationText.setFont(font);
    explanationText.setString(
        "Welcome to the Level Editor!\n\n"
        "Controls:\n"
        "1 - Buttons Mode\n"
        "2 - View Mode\n"
        "3 - Edit Mode\n"
        "Left Click - Place Tile\n"
        "Right Click - Remove Tile / Toggle Favorite\n"
        "Scroll - Change Tile\n"
        "F - Toggle Favorite\n"
        "Tab - Switch between Tiles and Favorites\n"
        "C - Copy Tile\n"
        "E - Toggle Tile Edit Mode\n"
        "S - Save Level\n"
        "Esc - Return to Menu\n\n"
        "Press any key to start."
    );
    explanationText.setCharacterSize(24);
    explanationText.setFillColor(sf::Color::White);
    explanationText.setPosition(50.0f, 50.0f);

    game.window.draw(explanationText);
}

void LevelCreator::drawInputBox(Game& game) {
    sf::RectangleShape overlay(sf::Vector2f(game.window.getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    game.window.draw(overlay);

    sf::RectangleShape inputBox(sf::Vector2f(400, 50));
    inputBox.setPosition(game.window.getSize().x / 2 - 200, game.window.getSize().y / 2 - 25);
    inputBox.setFillColor(sf::Color::White);
    game.window.draw(inputBox);

    sf::Text inputText;
    inputText.setFont(font);
    inputText.setString(inputFileName);
    inputText.setCharacterSize(24);
    inputText.setFillColor(sf::Color::Black);
    inputText.setPosition(inputBox.getPosition().x + 10, inputBox.getPosition().y + 10);
    game.window.draw(inputText);

    sf::Text promptText;
    promptText.setFont(font);
    promptText.setString(isSaving ? "Enter filename to save:" : "Enter filename to load:");
    promptText.setCharacterSize(24);
    promptText.setFillColor(sf::Color::White);
    promptText.setPosition(inputBox.getPosition().x, inputBox.getPosition().y - 30);
    game.window.draw(promptText);

    if (!isSaving) {
        // List existing levels
        sf::Text levelsText;
        levelsText.setFont(font);
        levelsText.setCharacterSize(20);
        levelsText.setFillColor(sf::Color::White);

        std::string levelsString = "Existing Levels:\n";
        try {
            for (const auto& entry : fs::directory_iterator("resources/Levels/")) {
                if (entry.is_regular_file()) {
                    levelsString += entry.path().filename().string() + "\n";
                }
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }
        levelsText.setString(levelsString);
        levelsText.setPosition(inputBox.getPosition().x, inputBox.getPosition().y + 60);

        game.window.draw(levelsText);
    }
}

void LevelCreator::saveLevelToCSV(const std::string& filename) {
    std::ofstream file("resources/Levels/" + filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file for saving" << std::endl;
        return;
    }

    file << boundaries.x << "," << boundaries.y << std::endl;

    for (size_t i = 0; i < placedTileSprites.size(); ++i) {
        for (int j = 0; j < placedTileSprites[i].size(); ++j) {
            file << i << "," << j << "," << placedTileIDs[i][j] << std::endl;
        }
    }

    file.close();
}

void LevelCreator::clearDrawing(Game& game) {
    placedTileSprites.clear();
    placedTileIDs.clear();

    game.changeState(std::make_shared<MenuState>());
}

void LevelCreator::loadLevelFromCSV(const std::string& filename, Game& game) {
    std::ifstream file("resources/Levels/" + filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file for loading" << std::endl;
        return;
    }

    // Read boundaries
    std::string line;
    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string xStr, yStr;
        if (std::getline(ss, xStr, ',') && std::getline(ss, yStr)) {
            boundaries.x = std::stoi(xStr);
            boundaries.y = std::stoi(yStr);
        }
    }

    placedTileIDs.clear();
    placedTileSprites.clear();

    placedTileIDs.resize(boundaries.x, std::vector<int>(boundaries.y, -1));
    placedTileSprites.resize(boundaries.x, std::vector<sf::Sprite>(boundaries.y));

    // Read tiles
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string iStr, jStr, tileIDStr;
        if (std::getline(ss, iStr, ',') && std::getline(ss, jStr, ',') && std::getline(ss, tileIDStr)) {
            int i = std::stoi(iStr);
            int j = std::stoi(jStr);
            int tileID = std::stoi(tileIDStr);

            if (i >= 0 && i < boundaries.x && j >= 0 && j < boundaries.y && tileID != -1) {
                float x = i * game.getTileSize();
                float y = j * game.getTileSize();

                sf::Sprite s;
                s.setTexture(tiles[tileID].getTexture());
                s.setScale(game.getTileSize() / s.getLocalBounds().height, game.getTileSize() / s.getLocalBounds().height);
                s.setPosition(x, y);

                placedTileIDs[i][j] = tileID;
                placedTileSprites[i][j] = s;
            }
        }
    }

    file.close();
}
