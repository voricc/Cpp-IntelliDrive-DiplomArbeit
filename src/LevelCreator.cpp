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

LevelCreator::LevelCreator(Game& game) : showExplanation(true) {
    initializeResources(game);
    createButtons(game);

    boundaries = {static_cast<int>(game.window.getSize().x / game.getTileSize()), static_cast<int>(game.window.getSize().y / game.getTileSize())};

    // Resize the vectors so they contain the necessary number of elements
    placedTileIDs.resize(boundaries.x, std::vector<int>(boundaries.y, -1));
    placedTileSprites.resize(boundaries.x, std::vector<sf::Sprite>(boundaries.y));
}

void LevelCreator::initializeResources(Game& game) {
    ResourceManager& resourceManager = ResourceManager::getInstance();

    resourceManager.loadTilesFromCSV("resources/Tiles/Tiles.csv");

    // Load the tiles through the resource Manager
    tiles = resourceManager.getTiles();

    resourceManager.loadFont("Rubik-Regular", "resources/Fonts/Rubik-Regular.ttf");
    font = resourceManager.getFont("Rubik-Regular");
/*
    resourceManager.loadTexture("background5", "resources/Backgrounds/background.webp");
    resourceManager.setTexture("backgroundlc", "resources/background.webp");
    sf::Texture& backgroundTexture = resourceManager.getTexture("backgroundlc");
    backgroundSprite.setTexture(backgroundTexture);

    sf::Vector2u windowSize = game.window.getSize();
    sf::Vector2u textureSize = backgroundTexture.getSize();
    backgroundSprite.setScale(
        static_cast<float>(windowSize.x) / textureSize.x,
        static_cast<float>(windowSize.y) / textureSize.y
    );
    */
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
            handleMouseInput(event, game);
            handleKeyboardInput(event, game);
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
            saveWallToCSV(inputFileName + ".csv");
            clearDrawing(game);
            inputActive = false;
        } else if (event.key.code == sf::Keyboard::Escape) {
            inputActive = false;
        }
    }
}

void LevelCreator::handleMouseInput(const sf::Event& event, Game& game) {
    sf::Vector2f mousePos = game.window.mapPixelToCoords(sf::Mouse::getPosition(game.window));

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            if (saveButton.getGlobalBounds().contains(mousePos)) {
                inputActive = true;
                inputFileName.clear();
            } else if (exitButton.getGlobalBounds().contains(mousePos)) {
                clearDrawing(game);
            } else {
                mouseDown = true;
                addTileAtMouse(game);
            }
        } else if (event.mouseButton.button == sf::Mouse::Right) {
            rightMouseDown = true;
            removeTileAtMouse(game);
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


void LevelCreator::handleKeyboardInput(const sf::Event& event, Game& game) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::S) {
            inputActive = true;
            inputFileName.clear();
        }
        if (event.key.code == sf::Keyboard::Escape) {
            game.changeState(std::make_shared<MenuState>());
        }
        if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
            selectedTile = (selectedTile + 1) % tiles.size();
        } else if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
            selectedTile = (selectedTile == 0) ? tiles.size() - 1 : selectedTile - 1;
        }
    }
}

void LevelCreator::addTileAtMouse(Game& game) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(game.window);

    sf::Vector2i grid(static_cast<int>(mousePos.x / game.getTileSize()), static_cast<int>(mousePos.y / game.getTileSize()));

    if (grid.x < 0 || grid.x > boundaries.x || grid.y < 0 || grid.y > boundaries.y){
        return; // Place no tiles if the cursor should be out of bounds
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

    if (grid.x < 0 || grid.x > boundaries.x || grid.y < 0 || grid.y > boundaries.y){
        return; // Place no tiles if the cursor should be out of bounds
    }

    placedTileIDs[grid.x][grid.y] = -1;
    placedTileSprites[grid.x][grid.y] = {};
}

void LevelCreator::update(Game& game) {
    if (showExplanation || inputActive) {
        return;
    }
    updatePreviewTile(game);
}

void LevelCreator::updatePreviewTile(Game& game) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(game.window);
    float snappedX = static_cast<int>(mousePos.x / game.getTileSize()) * game.getTileSize();
    float snappedY = static_cast<int>(mousePos.y / game.getTileSize()) * game.getTileSize();

    previewTile.setTexture(tiles[selectedTile].getTexture());
    previewTile.setScale(game.getTileSize() / previewTile.getLocalBounds().height, game.getTileSize() / previewTile.getLocalBounds().height);
    previewTile.setPosition(snappedX, snappedY);
    previewTile.setColor(sf::Color(255, 255, 255, 128));
}

void LevelCreator::render(Game& game) {
    game.window.clear();

    game.window.draw(backgroundSprite);

    drawPlacedTiles(game);

    if (!showExplanation && !inputActive) {
        game.window.draw(previewTile);
    }

    drawButtons(game);

    if (showExplanation) {
        drawExplanationScreen(game);
    }

    if (inputActive) {
        drawInputBox(game);
    }
}

void LevelCreator::drawPlacedTiles(Game& game) {
    for (const auto &tileX : placedTileSprites) {
        for(const auto &tileY : tileX) {
            game.window.draw(tileY);
        }
    }
}

void LevelCreator::drawButtons(Game& game) {
    game.window.draw(saveButton);
    game.window.draw(saveButtonText);

    game.window.draw(exitButton);
    game.window.draw(exitButtonText);
}

void LevelCreator::drawExplanationScreen(Game& game) {
    sf::RectangleShape overlay(sf::Vector2f(game.window.getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    game.window.draw(overlay);

    sf::Text explanationText;
    explanationText.setFont(font);
    explanationText.setString("Welcome to Level Creator!\n\n"
                              "Left-click to place tiles.\n"
                              "Right-click to delete tiles.\n"
                              "Use Left/Right arrows or A/D to change tiles.\n"
                              "Press any key to start.");
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
    promptText.setString("Enter filename:");
    promptText.setCharacterSize(24);
    promptText.setFillColor(sf::Color::White);
    promptText.setPosition(inputBox.getPosition().x, inputBox.getPosition().y - 30);
    game.window.draw(promptText);
}

void LevelCreator::saveWallToCSV(const std::string& filename) {
    std::ofstream file("resources/Levels/" + filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file to save" << std::endl;
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
