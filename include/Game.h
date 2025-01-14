//
// Created by Voric and tobisdev on 11/08/2024.
//

#ifndef GAME_H
#define GAME_H

#include <deque>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include "Car.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "State.h"
#include "ResourceManager.h"
#include "../vendors/ai/NeuralNetwork/NeuralNetwork.h"

class Game {
public:
    Game();
    ~Game();
    void run();

    void pushState(std::shared_ptr<State> state);
    void popState();
    void changeState(std::shared_ptr<State> state);
    std::shared_ptr<State> getCurrentState();

    void loadCarData(std::string path);
    void calculateAndDisplayFPS();

    Car& getCar() { return car; }

    sf::RenderWindow window;
    float dt;
    std::vector<carData> cars;
    int selectedCarIndex = 0;
    float getTileSize() {return this->tileSize;}

private:
    sf::Clock clock;
    Car car;
    sf::Event event;
    std::vector<std::shared_ptr<State>> states;
    std::deque<float> frameTimes;

    float tileSize = 64;

    sf::Font font;
    sf::Text fpsText, lowsText, avgText;

    const int maxFrameSamples = 300;

    void initializeText(sf::Text& text, float x, float y);
    void parseCarDataLine(const std::string& line, carData& data, ResourceManager& resourceManager);
    float calculateAverageFPS() const;
    float calculateOnePercentLowsFPS() const;
    void updateText(sf::Text& text, const std::string& label, float value);
};

#endif // GAME_H
