//
// Created by Voric on 11/08/2024.
//

#include "../include/PauseState.h"
#include "../include/ResourceManager.h"
#include "../include/MenuState.h"

PauseState::PauseState() {
    font = ResourceManager::getFont("Rubik-Regular");
    
    pauseText.setFont(font);
    pauseText.setString("Paused");
    pauseText.setCharacterSize(40);
    pauseText.setPosition(300, 150);

    resumeButton.setFont(font);
    resumeButton.setString("Press R to Resume");
    resumeButton.setCharacterSize(20);
    resumeButton.setPosition(250, 300);

    menuButton.setFont(font);
    menuButton.setString("Press M for Menu");
    menuButton.setCharacterSize(20);
    menuButton.setPosition(250, 350);
}

void PauseState::handleInput(Game& game) {
    sf::Event event;
    while (game.window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            game.window.close();

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
            game.popState();
        }

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M) {
            game.changeState(std::make_shared<MenuState>());
        }
    }
}

void PauseState::update(Game& game) {}

void PauseState::render(Game& game) {
    game.window.draw(pauseText);
    game.window.draw(resumeButton);
    game.window.draw(menuButton);
}