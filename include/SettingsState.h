//
// Created by Devrim on 08/12/2024.
//

#ifndef SETTINGSSTATE_H
#define SETTINGSSTATE_H

#include "State.h"
#include "Game.h"
#include "Dropdown.h"
#include <SFML/Graphics.hpp>

enum class SettingsTab {
    General,
    Graphics,
    Debug
};

class SettingsState : public State {
public:
    SettingsState();
    void handleInput(Game& game) override;
    void update(Game& game) override;
    void render(Game& game) override;

private:
    void switchTab(SettingsTab t);
    void applyChanges(Game& game);
    void loadUI();
    void drawTabs(sf::RenderWindow& window);
    void drawGeneral(sf::RenderWindow& window);
    void drawGraphics(sf::RenderWindow& window);
    void drawDebug(sf::RenderWindow& window);

    sf::Font font;
    sf::Text generalTabText;
    sf::Text graphicsTabText;
    sf::Text debugTabText;
    sf::Text saveButton;
    sf::Text backButton;

    SettingsTab currentTab;

    sf::RectangleShape panel;

    sf::Text aiModeText;
    sf::Text aiModeValue;

    sf::Text unitsText;
    Dropdown* unitsDropdown;

    Dropdown* displayModeDropdown;
    Dropdown* resolutionDropdown;
    sf::Text vsyncText;
    sf::Text vsyncValue;
    sf::Text fpsText;
    sf::Text fpsValue;

    sf::Text debugModeText;
    sf::Text debugModeValue;
    sf::Text fpsCounterText;
    sf::Text fpsCounterValue;
    sf::Text metricsModeText;
    Dropdown* metricsDropdown;

    bool hoverSave;
    bool hoverBack;
};

#endif