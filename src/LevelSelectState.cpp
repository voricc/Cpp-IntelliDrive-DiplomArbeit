 //
// Created by Devrim on 30.09.2024.
//

#include "../include/LevelSelectState.h"
#include "AIGameState.h"
#include "AiGameState.h"
#include "../include/GameState.h"
#include "../include/ResourceManager.h"

LevelSelectState::LevelSelectState() : currentPage(0), levelsPerPage(6) {
    defaultWindowSize = sf::Vector2u(1920, 1080);

    ResourceManager& resourceManager = ResourceManager::getInstance();
    resourceManager.loadFont("Rubik-Regular", "resources/Fonts/Rubik-Regular.ttf");
    resourceManager.loadFont("UpheavalPRO", "resources/Fonts/UpheavalPRO.ttf");

    font = resourceManager.getFont("Rubik-Regular");
    titlefont = resourceManager.getFont("UpheavalPRO");

    resourceManager.loadTexture("LevelSelectBackground", "resources/backgrounds/carchoosingstatebackground.png");
    backgroundTexture = resourceManager.getTexture("LevelSelectBackground");
    backgroundSprite.setTexture(backgroundTexture);

    titleText.setFont(titlefont);
    titleText.setString("LEVEL SELECTION");
    titleText.setCharacterSize(125);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition(defaultWindowSize.x / 2.0f - (titleText.getLocalBounds().width / 2), 40);

    resourceManager.loadTexture("ArrowLeft", "resources/GUI/arrowleft.png");
    resourceManager.loadTexture("ArrowRight", "resources/GUI/arrowright.png");

    arrowLeftTexture = resourceManager.getTexture("ArrowLeft");
    arrowRightTexture = resourceManager.getTexture("ArrowRight");
    arrowLeftSprite.setTexture(arrowLeftTexture);
    arrowRightSprite.setTexture(arrowRightTexture);

    loadLevelFiles();
    createLevelButtons();

    resourceManager.loadTilesFromCSV("resources/Tiles/Tiles.csv");
    tiles = resourceManager.getTiles();
}

void LevelSelectState::loadLevelFiles() {
    std::string path = "resources/Levels/";
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() == ".csv") {
            std::string filename = entry.path().filename().string();
            levelFiles.push_back(filename);
        }
    }
    totalPages = (levelFiles.size() + levelsPerPage - 1) / levelsPerPage;
}

void LevelSelectState::handleInput(Game& game) {
    sf::Event event;
    while (game.window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            game.window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(game.window);

            // Handle level selection
            for (int i = 0; i < levelsPerPage; ++i) {
                if (i + currentPage * levelsPerPage >= levelFiles.size()) break;
                if (levelButtons[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    std::string selectedLevel = "resources/Levels/" + levelFiles[i + currentPage * levelsPerPage];

                    // Dynamisch den Spielzustand bestimmen basierend auf AI_MODE und debug_mode
                    bool aiMode = VariableManager::getAiMode();
                    //bool debugMode = VariableManager::getD;

                    if (aiMode) {
                        game.changeState(std::make_shared<AiGameState>(game, selectedLevel/*, debugMode*/));
                    } else {
                        game.changeState(std::make_shared<GameState>(game, selectedLevel/*, debugMode*/));
                    }
                }
            }


            // Handle right arrow click (next page)
            if (arrowRightSprite.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                currentPage = (currentPage + 1) % totalPages;
                createLevelButtons();
            }

            // Handle left arrow click (previous page)
            if (arrowLeftSprite.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                currentPage = (currentPage - 1 + totalPages) % totalPages;
                createLevelButtons();
            }
        }
    }
}



void LevelSelectState::update(Game& game) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(game.window);
    for (auto& button : levelButtons) {
        button.setFillColor(sf::Color::White);
        if (button.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            button.setFillColor(sf::Color(255, 215, 0)); // Highlight button on hover
        }
    }
}

void LevelSelectState::render(Game& game) {
    game.window.clear();
    game.window.draw(backgroundSprite);
    game.window.draw(titleText);

    float previewWidth = 640.0f / 2.0f;
    float previewHeight = 360.0f / 2.0f;
    float verticalSpacing = 30.0f;

    for (int i = 0; i < levelsPerPage; ++i) {
        if (i + currentPage * levelsPerPage >= levelFiles.size()) break;

        int row = i / 2;
        int col = i % 2;

        sf::RectangleShape preview(sf::Vector2f(previewWidth, previewHeight));
        preview.setPosition(levelButtons[i].getPosition().x + (levelButtons[i].getSize().x - previewWidth) / 2,
                            levelButtons[i].getPosition().y + levelButtons[i].getSize().y + verticalSpacing);

        preview.setOutlineColor(sf::Color::White);
        preview.setOutlineThickness(1.0f);
        preview.setFillColor(sf::Color::Transparent);

        game.window.draw(levelButtons[i]);
        game.window.draw(levelTexts[i]);

        // Load and render preview elements
        loadLevelPreview(game, "resources/" + levelFiles[i + currentPage * levelsPerPage], preview);
        game.window.draw(preview);
    }

    // Draw navigation arrows
    game.window.draw(arrowRightSprite);
    game.window.draw(arrowLeftSprite);

}

void LevelSelectState::loadLevelPreview(Game& game, const std::string& filename, sf::RectangleShape& preview) {
    ResourceManager& resourceManager = ResourceManager::getInstance();

    if (cachedPreviews.find(filename) != cachedPreviews.end()) {
        for (const auto& element : cachedPreviews[filename]) {
            game.window.draw(element);
        }
        return;
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
       // std::cerr << "Error opening level preview: " << filename << std::endl;
        return;
    }

    std::string line;
    float previewWidth = preview.getSize().x;
    float previewHeight = preview.getSize().y;
    float levelWidth = 1920.0f;
    float levelHeight = 1080.0f;
    float scaleX = previewWidth / levelWidth;
    float scaleY = previewHeight / levelHeight;

    std::vector<sf::Sprite> previewElements;

    int textureIndex = 0;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string xStr, yStr;

        if (std::getline(ss, xStr, ',') && std::getline(ss, yStr, ',')) {
            try {
                if (!xStr.empty() && !yStr.empty()) {
                    float x = std::stof(xStr) * scaleX;
                    float y = std::stof(yStr) * scaleY;

                    sf::Sprite element;
                    element.setTexture(tiles[textureIndex % tiles.size()].getTexture());
                    element.setPosition(preview.getPosition().x + x, preview.getPosition().y + y);
                    element.setScale(scaleX, scaleY); // Scale the sprite to fit the preview

                    previewElements.push_back(element);
                    textureIndex++;


                    if (previewElements.size() > 10000) {
                        std::cerr << "Too many tiles in the preview!" << std::endl;
                        break;
                    }
                }
            } catch (const std::invalid_argument&) {
                std::cerr << "Invalid coordinate data in preview" << std::endl;
            } catch (const std::out_of_range&) {
                std::cerr << "Coordinate out of range in preview" << std::endl;
            }
        }
    }

    cachedPreviews[filename] = previewElements;
    file.close();

    for (const auto& element : previewElements) {
        game.window.draw(element);
    }
}

void LevelSelectState::createLevelButtons() {
    levelButtons.clear();
    levelTexts.clear();

    float windowHeight = 1080.0f;
    float windowWidth = 1920.0f;

    float buttonWidth = 400.0f;
    float buttonHeight = 50.0f;
    float previewSize = 200.0f;
    float verticalSpacing = (windowHeight - (2 * buttonHeight + 2 * previewSize)) / 3;

    for (int i = 0; i < levelsPerPage; ++i) {
        int row = i / 2;
        int col = i % 2;

        sf::RectangleShape button;
        button.setSize({buttonWidth, buttonHeight});
        button.setPosition(col * (windowWidth / 2) + (windowWidth / 4) - (buttonWidth / 2),
                           verticalSpacing + row * (buttonHeight + previewSize + verticalSpacing));
        button.setFillColor(sf::Color::Red);

        sf::Text buttonText;
        buttonText.setFont(font);
        buttonText.setFillColor(sf::Color::Black);
        buttonText.setCharacterSize(20);
        buttonText.setPosition(button.getPosition().x + 10.0f, button.getPosition().y + 10.0f);

        if (i + currentPage * levelsPerPage < levelFiles.size()) {
            buttonText.setString(levelFiles[i + currentPage * levelsPerPage]);
        } else {
            buttonText.setString("N/A");
        }

        levelButtons.push_back(button);
        levelTexts.push_back(buttonText);
    }


    float arrowScale = 0.1f;
    arrowLeftSprite.setScale(arrowScale, arrowScale);
    arrowRightSprite.setScale(arrowScale, arrowScale);

    arrowLeftSprite.setPosition(50.0f, windowHeight / 2 - arrowLeftSprite.getGlobalBounds().height / 2);
    arrowRightSprite.setPosition(windowWidth - arrowRightSprite.getGlobalBounds().width - 50.0f, windowHeight / 2 - arrowRightSprite.getGlobalBounds().height / 2);
}
