//
// Created by Voric on 11/08/2024.
//

#include "../include/DeathState.h"

#include "LevelSelectState.h"
#include "../include/GameState.h"
#include "../include/MenuState.h"
#include "../include/ResourceManager.h"

DeathState::DeathState() {
    font = ResourceManager::getFont("Rubik-Regular");

    initializeText(deathText, "You Crashed!", 40, 300.f, 150.f);
    initializeText(playAgainButton, "Press R to Play Again", 20, 250.f, 300.f);
    initializeText(menuButton, "Press M for Menu", 20, 250.f, 350.f);
}

void DeathState::handleInput(Game& game) {
    sf::Event event;
    while (game.window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            game.window.close();
        }
        if (isRestartKeyPressed(event)) {
            game.changeState(std::make_shared<LevelSelectState>());
        }
        if (isMenuKeyPressed(event)) {
            game.changeState(std::make_shared<MenuState>());
        }
    }
}

void DeathState::update(Game& game) {}

void DeathState::render(Game& game) {
    game.window.draw(deathText);
    game.window.draw(playAgainButton);
    game.window.draw(menuButton);
}

bool DeathState::isRestartKeyPressed(const sf::Event& event) const {
    return (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R);
}

bool DeathState::isMenuKeyPressed(const sf::Event& event) const {
    return (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M);
}

void DeathState::initializeText(sf::Text& text, const std::string& str, unsigned int size, float x, float y) {
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setPosition(x, y);
}
