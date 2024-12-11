//
// Created by Devrim on 08/12/2024.
//

#include "SettingsState.h"
#include "SettingsManager.h"
#include "MenuState.h"

SettingsState::SettingsState() : currentTab(SettingsTab::General), hoverSave(false), hoverBack(false) {
    font.loadFromFile("resources/Fonts/Rubik-Regular.ttf");

    generalTabText.setFont(font);
    generalTabText.setString("General");
    generalTabText.setCharacterSize(30);
    generalTabText.setPosition(200, 100);

    graphicsTabText.setFont(font);
    graphicsTabText.setString("Graphics");
    graphicsTabText.setCharacterSize(30);
    graphicsTabText.setPosition(400, 100);

    debugTabText.setFont(font);
    debugTabText.setString("Debug");
    debugTabText.setCharacterSize(30);
    debugTabText.setPosition(600, 100);

    saveButton.setFont(font);
    saveButton.setString("Save");
    saveButton.setCharacterSize(40);
    saveButton.setPosition(1920 - 200, 1080 - 100);

    backButton.setFont(font);
    backButton.setString("Back");
    backButton.setCharacterSize(40);
    backButton.setPosition(100, 1080 - 100);

    panel.setPosition(150,150);
    panel.setSize({1620,800});
    panel.setFillColor(sf::Color(30,30,30));
    panel.setOutlineColor(sf::Color::White);
    panel.setOutlineThickness(3);

    loadUI();
}

void SettingsState::loadUI() {
    SettingsManager& sm = SettingsManager::getInstance();

    aiModeText.setFont(font);
    aiModeText.setString("AI Mode");
    aiModeText.setCharacterSize(30);
    aiModeText.setPosition(200,200);

    aiModeValue.setFont(font);
    aiModeValue.setString(sm.getAIMode() ? "On" : "Off");
    aiModeValue.setCharacterSize(30);
    aiModeValue.setPosition(500,200);

    unitsText.setFont(font);
    unitsText.setString("Units");
    unitsText.setCharacterSize(30);
    unitsText.setPosition(200,250);
    unitsDropdown = new Dropdown(font, {"Metric","Imperial"}, 500,250,200,40);
    unitsDropdown->setSelectedItem(sm.getUnits());

    displayModeDropdown = new Dropdown(font, {"fullscreen","windowed","borderless"}, 500,200,250,40);
    displayModeDropdown->setSelectedItem(sm.getDisplayMode());

    resolutionDropdown = new Dropdown(font, {"1280x720","1920x1080","2560x1440"}, 500,250,250,40);
    resolutionDropdown->setSelectedItem(sm.getResolution());

    vsyncText.setFont(font);
    vsyncText.setString("VSync");
    vsyncText.setCharacterSize(30);
    vsyncText.setPosition(200,300);

    vsyncValue.setFont(font);
    vsyncValue.setString(sm.getVSync() ? "On" : "Off");
    vsyncValue.setCharacterSize(30);
    vsyncValue.setPosition(500,300);

    fpsText.setFont(font);
    fpsText.setString("FPS Limit");
    fpsText.setCharacterSize(30);
    fpsText.setPosition(200,350);

    fpsValue.setFont(font);
    fpsValue.setString(std::to_string(sm.getFPSLimit()));
    fpsValue.setCharacterSize(30);
    fpsValue.setPosition(500,350);

    debugModeText.setFont(font);
    debugModeText.setString("Debug Mode");
    debugModeText.setCharacterSize(30);
    debugModeText.setPosition(200,200);

    debugModeValue.setFont(font);
    debugModeValue.setString(sm.getDebugMode() ? "On" : "Off");
    debugModeValue.setCharacterSize(30);
    debugModeValue.setPosition(500,200);

    fpsCounterText.setFont(font);
    fpsCounterText.setString("FPS Counter");
    fpsCounterText.setCharacterSize(30);
    fpsCounterText.setPosition(200,250);

    fpsCounterValue.setFont(font);
    fpsCounterValue.setString(sm.getFPSCounter() ? "On" : "Off");
    fpsCounterValue.setCharacterSize(30);
    fpsCounterValue.setPosition(500,250);

    metricsModeText.setFont(font);
    metricsModeText.setString("Metrics Mode");
    metricsModeText.setCharacterSize(30);
    metricsModeText.setPosition(200,300);
    metricsDropdown = new Dropdown(font, {"simple","advanced"}, 500,300,200,40);
    metricsDropdown->setSelectedItem(sm.getMetricsMode());
}

void SettingsState::handleInput(Game& game) {
    sf::Event event;
    while (game.window.pollEvent(event)) {
        if(event.type == sf::Event::Closed) {
            game.window.close();
        }
        if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(game.window);
            if(generalTabText.getGlobalBounds().contains(mousePos.x, mousePos.y)) switchTab(SettingsTab::General);
            if(graphicsTabText.getGlobalBounds().contains(mousePos.x, mousePos.y)) switchTab(SettingsTab::Graphics);
            if(debugTabText.getGlobalBounds().contains(mousePos.x, mousePos.y)) switchTab(SettingsTab::Debug);

            if(saveButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                applyChanges();
                game.changeState(std::make_shared<MenuState>());
            }

            if(backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                game.changeState(std::make_shared<MenuState>());
            }

            if(currentTab == SettingsTab::General) {
                if(aiModeValue.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    aiModeValue.setString(aiModeValue.getString()=="On"?"Off":"On");
                }
                unitsDropdown->handleEvent(event, game.window);
            } else if(currentTab == SettingsTab::Graphics) {
                displayModeDropdown->handleEvent(event, game.window);
                resolutionDropdown->handleEvent(event, game.window);
                if(vsyncValue.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    vsyncValue.setString(vsyncValue.getString()=="On"?"Off":"On");
                }
                if(vsyncValue.getString() == "Off" && fpsValue.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    int val = std::stoi(fpsValue.getString().toAnsiString());
                    if(val==120) val=240; else if(val==240) val=360; else if(val==360) val=0; else val=120;
                    fpsValue.setString(std::to_string(val));
                }
            } else if(currentTab == SettingsTab::Debug) {
                if(debugModeValue.getGlobalBounds().contains(mousePos.x,mousePos.y)) {
                    debugModeValue.setString(debugModeValue.getString()=="On"?"Off":"On");
                }
                if(fpsCounterValue.getGlobalBounds().contains(mousePos.x,mousePos.y)) {
                    fpsCounterValue.setString(fpsCounterValue.getString()=="On"?"Off":"On");
                }
                metricsDropdown->handleEvent(event, game.window);
            }
        } else if(event.type == sf::Event::MouseMoved) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(game.window);
            hoverSave = saveButton.getGlobalBounds().contains(mousePos.x, mousePos.y);
            hoverBack = backButton.getGlobalBounds().contains(mousePos.x, mousePos.y);
        } else {
            if(currentTab == SettingsTab::General) {
                unitsDropdown->handleEvent(event, game.window);
            } else if(currentTab == SettingsTab::Graphics) {
                displayModeDropdown->handleEvent(event, game.window);
                resolutionDropdown->handleEvent(event, game.window);
            } else if(currentTab == SettingsTab::Debug) {
                metricsDropdown->handleEvent(event, game.window);
            }
        }
    }
}

void SettingsState::update(Game& game) {
}

void SettingsState::render(Game& game) {
    game.window.clear(sf::Color(20,20,20));
    drawTabs(game.window);
    game.window.draw(panel);

    if(currentTab == SettingsTab::General) {
        drawGeneral(game.window);
    } else if(currentTab == SettingsTab::Graphics) {
        drawGraphics(game.window);
    } else if(currentTab == SettingsTab::Debug) {
        drawDebug(game.window);
    }

    if(hoverSave) saveButton.setFillColor(sf::Color::Yellow); else saveButton.setFillColor(sf::Color::White);
    if(hoverBack) backButton.setFillColor(sf::Color::Yellow); else backButton.setFillColor(sf::Color::White);

    game.window.draw(saveButton);
    game.window.draw(backButton);
}

void SettingsState::drawTabs(sf::RenderWindow& window) {
    window.draw(generalTabText);
    window.draw(graphicsTabText);
    window.draw(debugTabText);
}

void SettingsState::drawGeneral(sf::RenderWindow& window) {
    window.draw(aiModeText);
    window.draw(aiModeValue);
    window.draw(unitsText);
    unitsDropdown->draw(window);
}

void SettingsState::drawGraphics(sf::RenderWindow& window) {
    sf::Text dmText("Display Mode", font, 30);
    dmText.setPosition(200,200);
    window.draw(dmText);
    displayModeDropdown->draw(window);

    sf::Text resText("Resolution", font, 30);
    resText.setPosition(200,250);
    window.draw(resText);
    resolutionDropdown->draw(window);

    window.draw(vsyncText);
    window.draw(vsyncValue);

    if(vsyncValue.getString()=="Off") {
        window.draw(fpsText);
        window.draw(fpsValue);
    }
}

void SettingsState::drawDebug(sf::RenderWindow& window) {
    window.draw(debugModeText);
    window.draw(debugModeValue);
    window.draw(fpsCounterText);
    window.draw(fpsCounterValue);
    window.draw(metricsModeText);
    metricsDropdown->draw(window);
}

void SettingsState::switchTab(SettingsTab t) {
    currentTab = t;
}

void SettingsState::applyChanges() {
    SettingsManager& sm = SettingsManager::getInstance();
    sm.setAIMode(aiModeValue.getString()=="On");
    sm.setUnits(unitsDropdown->getSelectedItem());
    sm.setDisplayMode(displayModeDropdown->getSelectedItem());
    sm.setResolution(resolutionDropdown->getSelectedItem());
    sm.setVSync(vsyncValue.getString()=="On");
    if(vsyncValue.getString()=="Off") {
        sm.setFPSLimit(std::stoi(fpsValue.getString().toAnsiString()));
    } else {
        sm.setFPSLimit(120);
    }
    sm.setDebugMode(debugModeValue.getString()=="On");
    sm.setFPSCounter(fpsCounterValue.getString()=="On");
    sm.setMetricsMode(metricsDropdown->getSelectedItem());
    sm.save("resources/config/config.json");
}