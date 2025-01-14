//
// Created by Voric and tobisdev on 31/08/2024.
//

#include "../include/CarChooseState.h"
#include "../include/Game.h"
#include "../include/GameState.h"
#include <iostream>
#include <MenuState.h>
#include <chrono>
#include <thread>
#include "../include/ResourceManager.h"

CarChoosingState::CarChoosingState() : selectedCarIndex(0) {
    defaultWindowSize = sf::Vector2u(1920, 1080);

    ResourceManager& resourceManager = ResourceManager::getInstance();

    resourceManager.loadFont("Rubik-Regular", "resources/Fonts/Rubik-Regular.ttf");
    resourceManager.loadFont("UpheavalPRO", "resources/Fonts/UpheavalPRO.ttf");

    font = resourceManager.getFont("Rubik-Regular");
    titlefont = resourceManager.getFont("UpheavalPRO");

    loadBackground();

    dividerline.setSize(sf::Vector2f(1920.0f, 5.0f));
    dividerline.setFillColor(sf::Color(80,80,80));
    dividerline.setPosition(0, 850);

    legendheight =  (dividerline.getGlobalBounds().height + dividerline.getPosition().y + 1080) / 2;

    titleText.setFont(titlefont);
    titleText.setString("CAR SELECTION");
    titleText.setCharacterSize(125);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition(defaultWindowSize.x / 2.0f - (titleText.getLocalBounds().width / 2), 40);

    driveTypeText.setFont(font);
    driveTypeText.setCharacterSize(30);
    driveTypeText.setFillColor(sf::Color::White);
    driveTypeText.setPosition(1000, 900);

    dividerline.setSize(sf::Vector2f(1920.0f, 5.0f));
    dividerline.setFillColor(sf::Color(80,80,80));
    dividerline.setPosition(0, 850);
}

void CarChoosingState::handleInput(Game& game) {
    sf::Event event;
    while (game.window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            game.window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
                selectedCarIndex = (selectedCarIndex - 1 + game.cars.size()) % game.cars.size();
            }
            if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
                selectedCarIndex = (selectedCarIndex + 1) % game.cars.size();
            }
            if (event.key.code == sf::Keyboard::Enter) {
                game.selectedCarIndex = selectedCarIndex;
                game.getCar().applyData(game.cars[selectedCarIndex]);
                game.changeState(std::make_shared<MenuState>());
            }
        }
    }
}

void CarChoosingState::update(Game& game) {
     carSprite = {};
     carSprite.setTexture(game.cars[selectedCarIndex].carTexture);
     carSprite.setColor(sf::Color::White);
     carSprite.setOrigin(carSprite.getLocalBounds().width / 2, carSprite.getLocalBounds().height / 2);
     carSprite.setPosition(game.window.getSize().x / 2, game.window.getSize().y / 2);
     carSprite.setScale(3.3f, 3.3f);

    rotationAngle += 0.9f;
    if (rotationAngle >= 360.0f) {
        rotationAngle = 0.0f;
    }
    carSprite.setRotation(rotationAngle);
}

void CarChoosingState::renderLogos(Game& game) {
    sf::Sprite sprite;
    sf::Text carNameText;

    sprite.setTexture(game.cars[selectedCarIndex].logoTexture);
    sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
    sf::Vector2f carPosition = carSprite.getPosition();
    sprite.setPosition(carPosition.x + carSprite.getLocalBounds().width / 2 - 200.0f, carPosition.y - 300.0f);
    sprite.setScale(0.1f, 0.1f);

    carNameText.setFont(font);
    carNameText.setString(game.cars[selectedCarIndex].name);
    carNameText.setCharacterSize(30);
    carNameText.setFillColor(sf::Color::White);

    float logoCenterY = sprite.getPosition().y;
    float textCenterY = carNameText.getLocalBounds().height / 2;
    carNameText.setPosition(sprite.getPosition().x + sprite.getGlobalBounds().width / 2 + 10.0f, logoCenterY - textCenterY);

    game.window.draw(sprite);
    game.window.draw(carNameText);
}

void CarChoosingState::loadBackground() {
    std::string backgroundImagePath = "resources/Backgrounds/carchoosingstatebackground.png";
    ResourceManager& resourceManager = ResourceManager::getInstance();

    resourceManager.loadTexture("CarChoosingBackground", backgroundImagePath);

    backgroundTexture = resourceManager.getTexture("CarChoosingBackground");
    backgroundSprite.setTexture(backgroundTexture);
}

void CarChoosingState::render(Game& game) {
    game.window.draw(backgroundSprite);
    game.window.draw(titleText);
    game.window.draw(carSprite);
    renderLogos(game);
    game.window.draw(dividerline);
    renderBottomLine(game);
}

void CarChoosingState::renderBottomLine(Game& game) {
    sf::Text statBarsLabel;

    statBarsLabel.setFont(font);
    statBarsLabel.setCharacterSize(25);
    statBarsLabel.setFillColor(sf::Color::White);

    carStatsText.setFont(font);
    carStatsText.setCharacterSize(25);
    carStatsText.setFillColor(sf::Color::White);

    driveTypeText.setFont(font);
    driveTypeText.setCharacterSize(30);
    driveTypeText.setFillColor(sf::Color::White);

    statBarsLabel.setString("Max Speed");
    float barWidth = 20.0f * game.cars[selectedCarIndex].MaxSpeed;
    sf::RectangleShape speedBar(sf::Vector2f(barWidth, 20.0f));
    speedBar.setFillColor(sf::Color::Red);
    speedBar.setPosition(315.0f, legendheight - 60);
    statBarsLabel.setPosition(300.0f - statBarsLabel.getGlobalBounds().width, legendheight - 65);
    game.window.draw(speedBar);
    game.window.draw(statBarsLabel);

    statBarsLabel.setString("Handling");
    barWidth = 20.0f * game.cars[selectedCarIndex].Handling;
    sf::RectangleShape handlingBar(sf::Vector2f(barWidth, 20.0f));
    handlingBar.setFillColor(sf::Color::Red);
    handlingBar.setPosition(315.0f, legendheight - 10);
    statBarsLabel.setPosition(300.0f - statBarsLabel.getGlobalBounds().width, legendheight - 15);
    game.window.draw(handlingBar);
    game.window.draw(statBarsLabel);

    statBarsLabel.setString("Acceleration");
    barWidth = 20.0f * game.cars[selectedCarIndex].Acceleration;
    sf::RectangleShape accelerationBar(sf::Vector2f(barWidth, 20.0f));
    accelerationBar.setFillColor(sf::Color::Red);
    accelerationBar.setPosition(315.0f, legendheight + 40);
    statBarsLabel.setPosition(300.0f - statBarsLabel.getGlobalBounds().width, legendheight + 35);
    float leftbounds = statBarsLabel.getPosition().x;
    game.window.draw(accelerationBar);
    game.window.draw(statBarsLabel);

    carStatsText.setString("Weight: " + std::to_string(game.cars[selectedCarIndex].weight) + " kg\n");
    carStatsText.setPosition(1920.0f - leftbounds - carStatsText.getGlobalBounds().width, legendheight - 65);
    game.window.draw(carStatsText);

    carStatsText.setString("Max Power: " + std::to_string(game.cars[selectedCarIndex].power) + " PS\n");
    carStatsText.setPosition(1920.0f - leftbounds - carStatsText.getGlobalBounds().width, legendheight - 15);
    game.window.draw(carStatsText);

    carStatsText.setString("Max Torque: " + std::to_string(game.cars[selectedCarIndex].torque) + " kgm");
    carStatsText.setPosition(1920.0f - leftbounds - carStatsText.getGlobalBounds().width, legendheight + 35);
    game.window.draw(carStatsText);

    driveTypeText.setPosition(800, legendheight - 15);
    driveTypeText.setString(game.cars[selectedCarIndex].driveType + " " + std::to_string(game.cars[selectedCarIndex].power) + " PS");

    game.window.draw(driveTypeText);
}
