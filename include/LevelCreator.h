//
// Created by Devrim on 30.09.2024.
//
// LevelCreator.h

#ifndef LEVELCREATOR_H
#define LEVELCREATOR_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Tile.h"
#include "GameState.h"

class Game;

class LevelCreator : public GameState {
public:
    LevelCreator(Game& game);

    void handleInput(Game& game) override;
    void update(Game& game) override;
    void render(Game& game) override;

private:
    enum class EditorState {
        Buttons,
        View,
        Edit
    };

    enum class TileEditMode {
        Place,
        Erase
    };

    void initializeResources(Game& game);
    void createButtons(Game& game);

    void handleExplanationInput(const sf::Event& event);
    void handleTextInput(const sf::Event& event, Game& game);
    void handleKeyboardInput(const sf::Event& event, Game& game);

    void handleEditInput(const sf::Event& event, Game& game);
    void handleViewInput(const sf::Event& event, Game& game);
    void handleButtonsInput(const sf::Event& event, Game& game);

    void updateEditState(Game& game);
    void updateViewState(Game& game);
    void updateButtonsState(Game& game);

    void updateTileSelectionUI(Game& game);
    void updatePreviewTile(Game& game);

    void renderEditState(Game& game);
    void renderViewState(Game& game);
    void renderButtonsState(Game& game);

    void addTileAtMouse(Game& game);
    void removeTileAtMouse(Game& game);

    void drawPlacedTiles(Game& game);
    void drawButtons(Game& game);
    void drawExplanationScreen(Game& game);
    void drawInputBox(Game& game);

    void saveLevelToCSV(const std::string& filename);
    void clearDrawing(Game& game);

    void loadLevelFromCSV(const std::string& filename, Game& game);

    sf::Sprite backgroundSprite;
    sf::Font font;

    sf::RectangleShape saveButton;
    sf::Text saveButtonText;
    sf::RectangleShape exitButton;
    sf::Text exitButtonText;
    sf::RectangleShape loadButton;
    sf::Text loadButtonText;

    bool mouseDown = false;
    bool rightMouseDown = false;
    bool showExplanation = true;
    bool inputActive = false;
    std::string inputFileName;
    bool isSaving = true;

    sf::Vector2i boundaries;

    std::vector<std::vector<int>> placedTileIDs;
    std::vector<std::vector<sf::Sprite>> placedTileSprites;

    std::vector<Tile> tiles;
    int selectedTile = 0;

    sf::Sprite previewTile;

    EditorState currentState = EditorState::Edit;

    std::vector<sf::Sprite> tileSelectionSprites;
    std::vector<sf::RectangleShape> tileSelectionBorders; // Added borders
    int tileSelectionRange = 2;

    bool selectingFavorites = false;
    std::vector<int> favoriteTiles;
    int selectedFavoriteIndex = 0;

    TileEditMode tileEditMode = TileEditMode::Place;

    sf::Text helpText;

    std::vector<int> tileIndices;
    int selectedIndex = 0;
    int totalTiles = 0;
};

#endif // LEVELCREATOR_H
