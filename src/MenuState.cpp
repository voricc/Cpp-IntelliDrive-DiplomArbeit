//
// Created by Voric and Tobias on 11/08/2024.
//

#include "../include/MenuState.h"
#include "CarChooseState.h"
#include "LevelSelectState.h"
#include "../include/GameState.h"
#include "../include/ResourceManager.h"

MenuState::MenuState()
    : backgroundIndex(1) {

    ResourceManager& resourceManager = ResourceManager::getInstance();

    std::string fontPath = "resources/Fonts/Rubik-Regular.ttf";
    std::string menuTitlePath = "resources/Fonts/MenuTitle-Font.ttf";

    resourceManager.loadFont("Rubik-Regular", fontPath);
    resourceManager.loadFont("MenuTitle-Font", menuTitlePath);

    Textfont = resourceManager.getFont("Rubik-Regular");
    Menufont = resourceManager.getFont("MenuTitle-Font");

    loadBackground();

    sf::Vector2u defaultWindowSize(1920, 1080);
    initializeButton(playButton, Textfont, "Start", defaultWindowSize, 70);
    initializeButton(carButton, Textfont, "Choose your Car", defaultWindowSize, 120);
    initializeButton(levelEditorButton, Textfont, "Level Editor", defaultWindowSize, 170);
    initializeButton(systemButton, Textfont, "Settings", defaultWindowSize, 220);
    initializeButton(exitButton, Textfont, "Exit", defaultWindowSize, 270);

    initializeText(changeBgButton, Textfont, "Change Background", 30, 150, defaultWindowSize.y - 30);
    initializeText(title, Menufont, "INTELLIDRIVE", 200, defaultWindowSize.x / 2.0f, defaultWindowSize.y / 4.0f + 100.f);

    initializeText(versionText, Textfont, "Beta v1.8.11", 20, defaultWindowSize.x - 150, defaultWindowSize.y - 100);
}

void MenuState::handleInput(Game& game) {
    sf::Event event;
    while (game.window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            game.window.close();
        }

        handleMouseInput(game);
    }
}

void MenuState::handleMouseInput(Game& game) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(game.window);

        if (playButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            game.changeState(std::make_shared<LevelSelectState>());
        } else if (carButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            game.changeState(std::make_shared<CarChoosingState>());
        } else if (levelEditorButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            game.changeState(std::make_shared<LevelCreator>(game));
        } else if (exitButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            game.window.close();
        } else if (changeBgButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            changeBackground();
        }
    }
}

void MenuState::update(Game& game) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(game.window);

    updateButtonHover(playButton, mousePos);
    updateButtonHover(levelEditorButton, mousePos);
    updateButtonHover(systemButton, mousePos);
    updateButtonHover(carButton, mousePos);
    updateButtonHover(exitButton, mousePos);
}

void MenuState::render(Game& game) {
    game.window.clear();
    game.window.draw(backgroundSprite);
    game.window.draw(title);
    game.window.draw(playButton);
    game.window.draw(levelEditorButton);
    game.window.draw(systemButton);
    game.window.draw(carButton);
    game.window.draw(exitButton);
    game.window.draw(changeBgButton);
    game.window.draw(versionText);
}

void MenuState::initializeButton(sf::Text& button, const sf::Font& font, const std::string& text, const sf::Vector2u& windowSize, float offsetY) {
    initializeText(button, font, text, 40, static_cast<float>(windowSize.x) / 2.0f, static_cast<float>(windowSize.y) / 2.0f + offsetY);
}

void MenuState::initializeText(sf::Text& textItem, const sf::Font& font, const std::string& text, unsigned int size, float x, float y) {
    textItem.setFont(font);
    textItem.setString(text);
    textItem.setCharacterSize(size);
    textItem.setFillColor(sf::Color::White);
    sf::FloatRect bounds = textItem.getLocalBounds();
    textItem.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    textItem.setPosition(x, y);
}

void MenuState::loadBackground() {
    ResourceManager& resourceManager = ResourceManager::getInstance();

    std::string backgroundImagePath = "resources/Backgrounds/background" + std::to_string(backgroundIndex) + ".png";

    resourceManager.loadTexture("MenuBackground_" + std::to_string(backgroundIndex), backgroundImagePath);

    backgroundTexture = resourceManager.getTexture("MenuBackground_" + std::to_string(backgroundIndex));
    backgroundSprite.setTexture(backgroundTexture);
}

void MenuState::changeBackground() {
    backgroundIndex = (backgroundIndex % 4) + 1;
    loadBackground();
}

void MenuState::updateButtonHover(sf::Text& button, const sf::Vector2i& mousePos) {
    if (button.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
        button.setFillColor(sf::Color(255, 215, 0));
        button.setScale(1.1f, 1.1f);
    } else {
        button.setFillColor(sf::Color::White);
        button.setScale(1.0f, 1.0f);
    }
}
