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
#include <cmath>      // For atan2

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
            sf::Vector2i mousePosInt = sf::Mouse::getPosition(game.window);
            sf::Vector2i grid(static_cast<int>(mousePosInt.x / game.getTileSize()), static_cast<int>(mousePosInt.y / game.getTileSize()));

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
        } else if (event.mouseButton.button == sf::Mouse::Middle) {
            sf::Vector2i grid(static_cast<int>(mousePos.x / game.getTileSize()), static_cast<int>(mousePos.y / game.getTileSize()));

            if (grid.x >= 0 && grid.x < boundaries.x && grid.y >= 0 && grid.y < boundaries.y) {
                // Start setting spawn point
                settingSpawnPoint = true;
                spawnPointPosition.x = grid.x * game.getTileSize() + game.getTileSize() / 2;
                spawnPointPosition.y = grid.y * game.getTileSize() + game.getTileSize() / 2;
                spawnPointDirection = sf::Vector2f(0.f, 0.f);
            }
        }
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            mouseDown = false;
        } else if (event.mouseButton.button == sf::Mouse::Right) {
            rightMouseDown = false;
        } else if (event.mouseButton.button == sf::Mouse::Middle) {
            if (settingSpawnPoint) {
                // Finalize the spawn point
                settingSpawnPoint = false;
                hasSpawnPoint = true;
            }
        }
    }

    if (event.type == sf::Event::MouseMoved) {
        if (settingSpawnPoint) {
            // Update spawnPointDirection with snapping
            sf::Vector2f mousePos = game.window.mapPixelToCoords(sf::Mouse::getPosition(game.window));
            sf::Vector2f rawDirection = mousePos - spawnPointPosition;

            if (rawDirection != sf::Vector2f(0.f, 0.f)) {
                // Calculate angle in degrees
                float angle = std::atan2(rawDirection.y, rawDirection.x) * 180.f / 3.14159265f;

                // Snap angle to nearest cardinal direction
                if (angle >= -45.f && angle < 45.f) {
                    // East
                    spawnPointDirection = sf::Vector2f(1.f, 0.f);
                } else if (angle >= 45.f && angle < 135.f) {
                    // South
                    spawnPointDirection = sf::Vector2f(0.f, 1.f);
                } else if (angle >= -135.f && angle < -45.f) {
                    // North
                    spawnPointDirection = sf::Vector2f(0.f, -1.f);
                } else {
                    // West
                    spawnPointDirection = sf::Vector2f(-1.f, 0.f);
                }
            }
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
    // Implementiere bei Bedarf
}

void LevelCreator::handleButtonsInput(const sf::Event& event, Game& game) {
    // Implementiere bei Bedarf
}

void LevelCreator::update(Game& game) {
    if (showExplanation || inputActive) {
        return;
    }

    // Update des Hilfetexts
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
    // Implementiere bei Bedarf
}

void LevelCreator::updateButtonsState(Game& game) {
    // Implementiere bei Bedarf
}

void LevelCreator::updateTileSelectionUI(Game& game) {
    int displaySize = 7; // Anzahl der anzuzeigenden Tiles
    tileSelectionRange = displaySize / 2;

    tileSelectionSprites.clear();
    tileSelectionBorders.clear(); // Lösche Ränder

    tileIndices.clear();
    if (selectingFavorites) {
        tileIndices = favoriteTiles;
    } else {
        tileIndices.resize(tiles.size());
        std::iota(tileIndices.begin(), tileIndices.end(), 0);
    }

    totalTiles = tileIndices.size();

    if (totalTiles == 0) return; // Keine Tiles zum Anzeigen

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

        // Erhöhe scaleFactor, um Tiles größer zu machen
        float scaleFactor = 1.2f;
        float scale = game.getTileSize() / sprite.getLocalBounds().height * scaleFactor;
        sprite.setScale(scale, scale);

        // Passe den Abstand an, um Überlappungen zu verhindern
        float spacing = game.getTileSize() * scaleFactor * 1.1f;
        float xPos = game.window.getSize().x / 2 + i * spacing;
        float yPos = game.window.getSize().y - game.getTileSize() * scaleFactor * 1.2f;
        sprite.setPosition(xPos, yPos);

        // Setze Transparenz (optional)
        int maxAlpha = 255;
        int minAlpha = 100;
        int alphaRange = maxAlpha - minAlpha;
        int alpha = maxAlpha - (std::abs(i) * (alphaRange / tileSelectionRange));
        if (alpha < minAlpha) alpha = minAlpha;
        sprite.setColor(sf::Color(255, 255, 255, alpha));

        tileSelectionSprites.push_back(sprite);

        // Erstelle ein Rechteck um den Sprite
        sf::FloatRect spriteBounds = sprite.getGlobalBounds();
        sf::RectangleShape border(sf::Vector2f(spriteBounds.width, spriteBounds.height));
        border.setPosition(spriteBounds.left, spriteBounds.top);
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineColor(sf::Color{40,40,40});
        border.setOutlineThickness(2.0f);
        // Kein Bedarf, die Origin zu setzen
        // tileSelectionBorders.push_back(border);
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

    // Zeichne den Hilfetext ohne Hintergrund
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

    // Berechne das umgebende Rechteck der Tile-Auswahl-Sprites
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

        // Erstelle Hintergrundrechteck ähnlich dem Hilfetext
        sf::RectangleShape tileSelectionBackground(sf::Vector2f(boundingRect.width + 20, boundingRect.height + 20));
        tileSelectionBackground.setPosition(boundingRect.left - 10, boundingRect.top - 10);
        tileSelectionBackground.setFillColor(sf::Color(0, 0, 0, 150));
        game.window.draw(tileSelectionBackground);
    }

    // Zeichne die Tile-Auswahl-UI
    for (const auto& sprite : tileSelectionSprites) {
        game.window.draw(sprite);
    }
    // Zeichne die Ränder
    for (const auto& border : tileSelectionBorders) {
        game.window.draw(border);
    }

    // Zeichne das Vorschau-Tile
    if (selectedTile != -1) {
        game.window.draw(previewTile);
    }

    // Zeichne den Spawnpoint, falls vorhanden
    if (hasSpawnPoint) {
        // Zeichne einen hellgrünen Kreis an der Spawnpoint-Position
        sf::CircleShape spawnPointCircle(5.f);
        spawnPointCircle.setFillColor(sf::Color(0, 255, 0)); // Hellgrün
        spawnPointCircle.setOrigin(5.f, 5.f);
        spawnPointCircle.setPosition(spawnPointPosition);

        game.window.draw(spawnPointCircle);

        // Zeichne einen Pfeil, der die Spawnpoint-Richtung anzeigt
        if (spawnPointDirection != sf::Vector2f(0.f, 0.f)) {
            sf::Vector2f normalizedDir = spawnPointDirection;
            float length = std::sqrt(normalizedDir.x * normalizedDir.x + normalizedDir.y * normalizedDir.y);
            if (length != 0.f) {
                normalizedDir /= length;
            }

            sf::Vector2f endPoint = spawnPointPosition + normalizedDir * 30.f; // Pfeillänge

            // Zeichne den Linienabschnitt des Pfeils
            sf::Vertex line[] = {
                sf::Vertex(spawnPointPosition, sf::Color(0, 255, 0)),
                sf::Vertex(endPoint, sf::Color(0, 255, 0))
            };
            game.window.draw(line, 2, sf::Lines);

            // Zeichne die Pfeilspitze
            float arrowHeadSize = 10.f;
            sf::Vector2f perp(-normalizedDir.y, normalizedDir.x); // Senkrechter Vektor

            sf::Vertex arrowHead[] = {
                sf::Vertex(endPoint, sf::Color(0, 255, 0)),
                sf::Vertex(endPoint - normalizedDir * arrowHeadSize + perp * arrowHeadSize / 2.f, sf::Color(0, 255, 0)),
                sf::Vertex(endPoint - normalizedDir * arrowHeadSize - perp * arrowHeadSize / 2.f, sf::Color(0, 255, 0))
            };
            game.window.draw(arrowHead, 3, sf::Triangles);
        }
    }
}

void LevelCreator::renderViewState(Game& game) {
    drawPlacedTiles(game);
    // Implementiere zusätzliche Rendering-Logik bei Bedarf
}

void LevelCreator::renderButtonsState(Game& game) {
    drawPlacedTiles(game);
    // Implementiere zusätzliche Rendering-Logik bei Bedarf
}

void LevelCreator::addTileAtMouse(Game& game) {
    if (selectedTile == -1) {
        return; // Kein Tile zum Platzieren
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
        "Willkommen im Level Editor!\n\n"
        "Steuerung:\n"
        "1 - Buttons Modus\n"
        "2 - View Modus\n"
        "3 - Edit Modus\n"
        "Linksklick - Tile platzieren\n"
        "Rechtsklick - Tile entfernen / Favorit toggeln\n"
        "Scrollen - Tile wechseln\n"
        "F - Favorit toggeln\n"
        "Tab - Zwischen Tiles und Favoriten wechseln\n"
        "C - Tile kopieren\n"
        "E - Tile Edit Modus wechseln\n"
        "Mittlere Maustaste - Spawnpunkt setzen\n"
        "S - Level speichern\n"
        "Esc - Zurück zum Menü\n\n"
        "Drücke eine beliebige Taste, um zu starten."
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
        // Liste vorhandener Levels
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

    // Speichere Spawnpoint-Daten
    if (hasSpawnPoint) {
        file << "SPAWN_POINT," << spawnPointPosition.x << "," << spawnPointPosition.y << ","
             << spawnPointDirection.x << "," << spawnPointDirection.y << std::endl;
    }

    // Speichere Tile-Daten
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

    // Lese Grenzen
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

    // Lese Tiles
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string firstToken;
        std::getline(ss, firstToken, ',');

        if (firstToken == "SPAWN_POINT") {
            // Lese Spawnpoint-Daten
            std::string xStr, yStr, dirXStr, dirYStr;
            std::getline(ss, xStr, ',');
            std::getline(ss, yStr, ',');
            std::getline(ss, dirXStr, ',');
            std::getline(ss, dirYStr);

            spawnPointPosition.x = std::stof(xStr);
            spawnPointPosition.y = std::stof(yStr);
            spawnPointDirection.x = std::stof(dirXStr);
            spawnPointDirection.y = std::stof(dirYStr);
            hasSpawnPoint = true;
        } else {
            // Lese Tile-Daten
            std::string iStr = firstToken;
            std::string jStr, tileIDStr;
            if (std::getline(ss, jStr, ',') && std::getline(ss, tileIDStr)) {
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
    }

    file.close();
}
