//
// Created by Tobias on 14.01.2025.
//

#include "SettingsState.h"

SettingsState::SettingsState()
        : currentTab(SettingsTab::General), hoverSave(false), hoverBack(false),
          hoverValueColor(sf::Color::Yellow), hoveredTabElement(nullptr)
{
    font = ResourceManager::getFont("Rubik-Regular");

    generalTabText.setFont(font);
    generalTabText.setString("General");
    generalTabText.setCharacterSize(30);
    generalTabText.setPosition(200, 100);

    graphicsTabText.setFont(font);
    graphicsTabText.setString("Graphics");
    graphicsTabText.setCharacterSize(30);
    graphicsTabText.setPosition(400, 100);

    physicsTabText.setFont(font);
    physicsTabText.setString("Physics");
    physicsTabText.setCharacterSize(30);
    physicsTabText.setPosition(600, 100);

    aiTabText.setFont(font);
    aiTabText.setString("AI");
    aiTabText.setCharacterSize(30);
    aiTabText.setPosition(800, 100);

    evoTabText.setFont(font);
    evoTabText.setString("Evolutionary");
    evoTabText.setCharacterSize(30);
    evoTabText.setPosition(1000, 100);

    debugTabText.setFont(font);
    debugTabText.setString("Debug");
    debugTabText.setCharacterSize(30);
    debugTabText.setPosition(1200, 100);

    saveButton.setFont(font);
    saveButton.setString("Save");
    saveButton.setCharacterSize(40);
    saveButton.setPosition(1700, 1000);

    backButton.setFont(font);
    backButton.setString("Back");
    backButton.setCharacterSize(40);
    backButton.setPosition(100, 1000);

    panel.setPosition(150, 150);
    panel.setSize({1620, 800});
    panel.setFillColor(sf::Color(30, 30, 30));
    panel.setOutlineColor(sf::Color::White);
    panel.setOutlineThickness(3);

    unitsDropdown = nullptr;
    displayModeDropdown = nullptr;
    resolutionDropdown = nullptr;
    metricsDropdown = nullptr;

    loadUI();
}

void SettingsState::setupText(sf::Text& txt, const std::string& str, float x, float y) {
    txt.setFont(font);
    txt.setString(str);
    txt.setCharacterSize(30);
    txt.setPosition(x, y);
}

std::string SettingsState::floatToStringTrimZero(float val) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(3) << val;
    std::string s = ss.str();
    while (!s.empty() && s.back() == '0') {
        s.pop_back();
    }
    if (!s.empty() && s.back() == '.') {
        s.pop_back();
    }
    return s;
}

void SettingsState::loadUI() {
    {
        TabElement aiElem;
        setupText(aiElem.label, "AI Mode", LABEL_X, START_Y);
        bool aiMode = VariableManager::getAiMode();
        setupText(aiElem.value, aiMode ? "On" : "Off", VALUE_X, START_Y);
        aiElem.isBool = true;
        generalData.elements.push_back(aiElem);

        TabElement unitsLabel;
        setupText(unitsLabel.label, "Units", LABEL_X, START_Y + GAP_Y);
        unitsLabel.isDropdown = true;
        generalData.elements.push_back(unitsLabel);

        unitsDropdown = new Dropdown(
                font,
                {"Metric", "Imperial"},
                DROPDOWN_X,
                START_Y + GAP_Y,
                DROPDOWN_WIDTH,
                DROPDOWN_HEIGHT
        );
        unitsDropdown->setSelectedItem(VariableManager::getUnits());
        generalData.dropdown = unitsDropdown;
    }

    {
        TabElement displayLabel;
        setupText(displayLabel.label, "Display Mode", LABEL_X, START_Y);
        displayLabel.isDropdown = true;
        graphicsData.elements.push_back(displayLabel);

        displayModeDropdown = new Dropdown(
                font,
                {"fullscreen","windowed","borderless"},
                DROPDOWN_X,
                START_Y,
                DROPDOWN_WIDTH,
                DROPDOWN_HEIGHT
        );
        displayModeDropdown->setSelectedItem(VariableManager::getDisplayMode());
        graphicsData.dropdown = displayModeDropdown;

        TabElement resolutionLabel;
        setupText(resolutionLabel.label, "Resolution", LABEL_X, START_Y + GAP_Y);
        resolutionLabel.isDropdown = true;
        graphicsData.elements.push_back(resolutionLabel);

        resolutionDropdown = new Dropdown(
                font,
                {"1280x720","1920x1080","2560x1440"},
                DROPDOWN_X,
                START_Y + GAP_Y,
                DROPDOWN_WIDTH,
                DROPDOWN_HEIGHT
        );
        resolutionDropdown->setSelectedItem(VariableManager::getResolution());
        graphicsData.dropdown2 = resolutionDropdown;

        TabElement vsyncElem;
        setupText(vsyncElem.label, "VSync", LABEL_X, START_Y + 2 * GAP_Y);
        bool vsyncFlag = VariableManager::getVSync();
        setupText(vsyncElem.value, vsyncFlag ? "On" : "Off", VALUE_X, START_Y + 2 * GAP_Y);
        vsyncElem.isBool = true;
        graphicsData.elements.push_back(vsyncElem);

        TabElement fpsElem;
        setupText(fpsElem.label, "FPS Limit", LABEL_X, START_Y + 3 * GAP_Y);
        fpsElem.numericValue = static_cast<float>(VariableManager::getFpsLimit());
        setupText(fpsElem.value, std::to_string(VariableManager::getFpsLimit()), VALUE_X, START_Y + 3 * GAP_Y);
        fpsElem.isFloat = false;
        fpsElem.step = 30.f;
        graphicsData.elements.push_back(fpsElem);
    }

    {
        TabElement acc;
        setupText(acc.label, "Max Acceleration", LABEL_X, START_Y);
        acc.numericValue = VariableManager::getMaxAccelerationConstant();
        setupText(acc.value, floatToStringTrimZero(acc.numericValue), VALUE_X, START_Y);
        physicsData.elements.push_back(acc);

        TabElement angAcc;
        setupText(angAcc.label, "Max Ang Acceleration", LABEL_X, START_Y + GAP_Y);
        angAcc.numericValue = VariableManager::getMaxAngularAccelerationConstant();
        setupText(angAcc.value, floatToStringTrimZero(angAcc.numericValue), VALUE_X, START_Y + GAP_Y);
        physicsData.elements.push_back(angAcc);

        TabElement maxSpd;
        setupText(maxSpd.label, "Max Speed", LABEL_X, START_Y + 2 * GAP_Y);
        maxSpd.numericValue = VariableManager::getMaxSpeed();
        setupText(maxSpd.value, floatToStringTrimZero(maxSpd.numericValue), VALUE_X, START_Y + 2 * GAP_Y);
        physicsData.elements.push_back(maxSpd);

        TabElement rotMult;
        setupText(rotMult.label, "Rotational Speed Mult", LABEL_X, START_Y + 3 * GAP_Y);
        rotMult.numericValue = VariableManager::getRotationalSpeedMultiplier();
        rotMult.step = 0.001f;
        setupText(rotMult.value, floatToStringTrimZero(rotMult.numericValue), VALUE_X, START_Y + 3 * GAP_Y);
        physicsData.elements.push_back(rotMult);

        TabElement angDamp;
        setupText(angDamp.label, "Angular Dampening", LABEL_X, START_Y + 4 * GAP_Y);
        angDamp.numericValue = VariableManager::getAngularDampingMultiplier();
        angDamp.step = 0.01f;
        setupText(angDamp.value, floatToStringTrimZero(angDamp.numericValue), VALUE_X, START_Y + 4 * GAP_Y);
        physicsData.elements.push_back(angDamp);
    }

    {
        TabElement rays;
        setupText(rays.label, "Rays", LABEL_X, START_Y);
        rays.numericValue = static_cast<float>(VariableManager::getRayAmount());
        rays.isFloat = false;
        setupText(rays.value, std::to_string(VariableManager::getRayAmount()), VALUE_X, START_Y);
        aiData.elements.push_back(rays);

        TabElement fov;
        setupText(fov.label, "Fov", LABEL_X, START_Y + GAP_Y);
        fov.numericValue = VariableManager::getAiFov();
        setupText(fov.value, floatToStringTrimZero(fov.numericValue), VALUE_X, START_Y + GAP_Y);
        aiData.elements.push_back(fov);

        TabElement nets;
        setupText(nets.label, "Networks", LABEL_X, START_Y + 2 * GAP_Y);
        nets.numericValue = static_cast<float>(VariableManager::getNetworksAmount());
        nets.isFloat = false;
        setupText(nets.value, std::to_string(VariableManager::getNetworksAmount()), VALUE_X, START_Y + 2 * GAP_Y);
        aiData.elements.push_back(nets);

        TabElement randMax;
        setupText(randMax.label, "Random Values Max", LABEL_X, START_Y + 3 * GAP_Y);
        randMax.numericValue = VariableManager::getAiInitialRandomValuesMax();
        setupText(randMax.value, floatToStringTrimZero(randMax.numericValue), VALUE_X, START_Y + 3 * GAP_Y);
        aiData.elements.push_back(randMax);

        TabElement randUni;
        setupText(randUni.label, "Uniform Distribution", LABEL_X, START_Y + 4 * GAP_Y);
        bool uni = VariableManager::getAiInitialRandomValuesUniform();
        setupText(randUni.value, uni ? "On" : "Off", VALUE_X, START_Y + 4 * GAP_Y);
        randUni.isBool = true;
        aiData.elements.push_back(randUni);
    }

    {
        TabElement winners;
        setupText(winners.label, "AI Winners", LABEL_X, START_Y);
        winners.numericValue = static_cast<float>(VariableManager::getAiWinners());
        winners.isFloat = false;
        setupText(winners.value, std::to_string(VariableManager::getAiWinners()), VALUE_X, START_Y);
        evoData.elements.push_back(winners);

        TabElement cpPoints;
        setupText(cpPoints.label, "Checkpoint Reward", LABEL_X, START_Y + GAP_Y);
        cpPoints.numericValue = VariableManager::getCheckpointPoints();
        setupText(cpPoints.value, floatToStringTrimZero(cpPoints.numericValue), VALUE_X, START_Y + GAP_Y);
        evoData.elements.push_back(cpPoints);

        TabElement cpRadius;
        setupText(cpRadius.label, "Checkpoint Radius", LABEL_X, START_Y + 3 * GAP_Y);
        cpRadius.numericValue = VariableManager::getCheckpointRadius();
        setupText(cpRadius.value, floatToStringTrimZero(cpRadius.numericValue), VALUE_X, START_Y + 3 * GAP_Y);
        evoData.elements.push_back(cpRadius);

        TabElement rotPen;
        setupText(rotPen.label, "Rotation Penalty", LABEL_X, START_Y + 2 * GAP_Y);
        rotPen.numericValue = VariableManager::getRotationPenalty();
        setupText(rotPen.value, floatToStringTrimZero(rotPen.numericValue), VALUE_X, START_Y + 2 * GAP_Y);
        evoData.elements.push_back(rotPen);

        TabElement backPen;
        setupText(backPen.label, "Backwards Penalty", LABEL_X, START_Y + 4 * GAP_Y);
        backPen.numericValue = VariableManager::getBackwardsMovementPenalty();
        setupText(backPen.value, floatToStringTrimZero(backPen.numericValue), VALUE_X, START_Y + 4 * GAP_Y);
        evoData.elements.push_back(backPen);

        TabElement restart;
        setupText(restart.label, "Restart %", LABEL_X, START_Y + 5 * GAP_Y);
        restart.numericValue = VariableManager::getRestartOnDeadPercentage();
        setupText(restart.value, floatToStringTrimZero(restart.numericValue), VALUE_X, START_Y + 5 * GAP_Y);
        evoData.elements.push_back(restart);

        TabElement mut;
        setupText(mut.label, "Mutation Index", LABEL_X, START_Y + 6 * GAP_Y);
        mut.numericValue = VariableManager::getMutationIndex();
        setupText(mut.value, floatToStringTrimZero(mut.numericValue), VALUE_X, START_Y + 6 * GAP_Y);
        evoData.elements.push_back(mut);
    }

    {
        TabElement fpsC;
        setupText(fpsC.label, "FPS Counter", LABEL_X, START_Y);
        bool fc = VariableManager::getFpsCounter();
        setupText(fpsC.value, fc ? "On" : "Off", VALUE_X, START_Y);
        fpsC.isBool = true;
        debugData.elements.push_back(fpsC);

        TabElement showColl;
        setupText(showColl.label, "Show Colliders", LABEL_X, START_Y + GAP_Y);
        bool sc = VariableManager::getShowColliders();
        setupText(showColl.value, sc ? "On" : "Off", VALUE_X, START_Y + GAP_Y);
        showColl.isBool = true;
        debugData.elements.push_back(showColl);

        TabElement showCheck;
        setupText(showCheck.label, "Show Checkpoints", LABEL_X, START_Y + 2 * GAP_Y);
        bool sck = VariableManager::getShowCheckpoints();
        setupText(showCheck.value, sck ? "On" : "Off", VALUE_X, START_Y + 2 * GAP_Y);
        showCheck.isBool = true;
        debugData.elements.push_back(showCheck);

        TabElement showRay;
        setupText(showRay.label, "Show Rays", LABEL_X, START_Y + 3 * GAP_Y);
        bool sr = VariableManager::getShowRays();
        setupText(showRay.value, sr ? "On" : "Off", VALUE_X, START_Y + 3 * GAP_Y);
        showRay.isBool = true;
        debugData.elements.push_back(showRay);

        TabElement carsPerc;
        setupText(carsPerc.label, "Show Cars", LABEL_X, START_Y + 4 * GAP_Y);
        carsPerc.numericValue = VariableManager::getShowCarsPercentage();
        setupText(carsPerc.value, floatToStringTrimZero(carsPerc.numericValue), VALUE_X, START_Y + 4 * GAP_Y);
        debugData.elements.push_back(carsPerc);

        TabElement metricsLabel;
        setupText(metricsLabel.label, "Metrics Mode", LABEL_X, START_Y + 5 * GAP_Y);
        metricsLabel.isDropdown = true;
        debugData.elements.push_back(metricsLabel);

        metricsDropdown = new Dropdown(
                font,
                {"simple","advanced"},
                DROPDOWN_X,
                START_Y + 5 * GAP_Y,
                DROPDOWN_WIDTH,
                DROPDOWN_HEIGHT
        );
        metricsDropdown->setSelectedItem(VariableManager::getMetricsMode());
        debugData.dropdown = metricsDropdown;
    }
}

void SettingsState::switchTab(SettingsTab t) {
    currentTab = t;
}

void SettingsState::drawTabs(sf::RenderWindow& window) {
    window.draw(generalTabText);
    window.draw(graphicsTabText);
    window.draw(physicsTabText);
    window.draw(aiTabText);
    window.draw(evoTabText);
    window.draw(debugTabText);
}

void SettingsState::drawTabData(sf::RenderWindow& window, TabData& data) {
    for (auto& elem : data.elements) {
        if (&elem == hoveredTabElement) {
            elem.value.setFillColor(hoverValueColor);
        } else {
            elem.value.setFillColor(sf::Color::White);
        }
        window.draw(elem.label);
        window.draw(elem.value);
    }
    if (data.dropdown)  data.dropdown->draw(window);
    if (data.dropdown2) data.dropdown2->draw(window);
}

void SettingsState::drawGeneral(sf::RenderWindow& window) {
    drawTabData(window, generalData);
}

void SettingsState::drawGraphics(sf::RenderWindow& window) {
    drawTabData(window, graphicsData);
}

void SettingsState::drawPhysics(sf::RenderWindow& window) {
    drawTabData(window, physicsData);
}

void SettingsState::drawAI(sf::RenderWindow& window) {
    drawTabData(window, aiData);
}

void SettingsState::drawEvolutionary(sf::RenderWindow& window) {
    drawTabData(window, evoData);
}

void SettingsState::drawDebug(sf::RenderWindow& window) {
    drawTabData(window, debugData);
}

void SettingsState::handleTabElementClick(TabElement& elem, sf::Vector2i mousePos, bool isRightClick) {
    if (!elem.value.getGlobalBounds().contains(mousePos.x, mousePos.y)) return;
    if (elem.isBool) {
        std::string current = elem.value.getString();
        elem.value.setString(current == "On" ? "Off" : "On");
    }
    else if (!elem.isDropdown) {
        float v = elem.numericValue;
        float localStep = elem.step;
        if (isRightClick) localStep = -localStep;
        v += localStep;
        if (v < elem.minVal) v = elem.maxVal;
        if (v > elem.maxVal) v = elem.minVal;
        elem.numericValue = v;
        if (elem.isFloat) {
            elem.value.setString(floatToStringTrimZero(v));
        } else {
            elem.value.setString(std::to_string(static_cast<int>(v)));
        }
    }
}

float SettingsState::getScrollStep(float val) {
    float absVal = std::fabs(val);
    if (absVal >= 1.f) {
        int digits = 0;
        int intPart = static_cast<int>(absVal);
        while (intPart >= 10) {
            intPart /= 10;
            digits++;
        }
        float base = 1.f;
        for (int i = 0; i < digits; ++i) {
            base *= 10.f;
        }
        return base;
    } else {
        int decimalPlaces = 0;
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(6) << absVal;
        std::string s = oss.str();
        size_t pos = s.find('.') != std::string::npos ? s.find('.') : s.size();
        size_t idx = pos + 1;
        while (idx < s.size() && s[idx] == '0') {
            decimalPlaces++;
            idx++;
        }
        return std::pow(10.f, -(decimalPlaces + 1));
    }
}

void SettingsState::handleInput(Game& game) {
    sf::Event event;
    while (game.window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            game.window.close();
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(game.window);
            bool isRightClick = (event.mouseButton.button == sf::Mouse::Right);

            if (generalTabText.getGlobalBounds().contains(mousePos.x, mousePos.y))      switchTab(SettingsTab::General);
            else if (graphicsTabText.getGlobalBounds().contains(mousePos.x, mousePos.y))switchTab(SettingsTab::Graphics);
            else if (physicsTabText.getGlobalBounds().contains(mousePos.x, mousePos.y)) switchTab(SettingsTab::Physics);
            else if (aiTabText.getGlobalBounds().contains(mousePos.x, mousePos.y))      switchTab(SettingsTab::AI);
            else if (evoTabText.getGlobalBounds().contains(mousePos.x, mousePos.y))     switchTab(SettingsTab::Evolutionary);
            else if (debugTabText.getGlobalBounds().contains(mousePos.x, mousePos.y))   switchTab(SettingsTab::Debug);

            if (saveButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                applyChanges(game);
                game.changeState(std::make_shared<MenuState>());
            }
            else if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                game.changeState(std::make_shared<MenuState>());
            }

            auto processTab = [&](TabData& tabData){
                for (auto& e : tabData.elements) {
                    handleTabElementClick(e, mousePos, isRightClick);
                }
                if (tabData.dropdown)  tabData.dropdown->handleEvent(event, game.window);
                if (tabData.dropdown2) tabData.dropdown2->handleEvent(event, game.window);
            };

            switch (currentTab) {
                case SettingsTab::General:     processTab(generalData);     break;
                case SettingsTab::Graphics:    processTab(graphicsData);    break;
                case SettingsTab::Physics:     processTab(physicsData);     break;
                case SettingsTab::AI:          processTab(aiData);          break;
                case SettingsTab::Evolutionary:processTab(evoData);         break;
                case SettingsTab::Debug:       processTab(debugData);       break;
            }
        }
        else if (event.type == sf::Event::MouseMoved) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(game.window);
            hoverSave = saveButton.getGlobalBounds().contains(mousePos.x, mousePos.y);
            hoverBack = backButton.getGlobalBounds().contains(mousePos.x, mousePos.y);
            hoveredTabElement = nullptr;

            auto checkHover = [&](TabData& tabData){
                for (auto& e : tabData.elements) {
                    if (e.value.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        hoveredTabElement = &e;
                        break;
                    }
                }
                if (tabData.dropdown)  tabData.dropdown->handleEvent(event, game.window);
                if (tabData.dropdown2) tabData.dropdown2->handleEvent(event, game.window);
            };

            switch (currentTab) {
                case SettingsTab::General:     checkHover(generalData);     break;
                case SettingsTab::Graphics:    checkHover(graphicsData);    break;
                case SettingsTab::Physics:     checkHover(physicsData);     break;
                case SettingsTab::AI:          checkHover(aiData);          break;
                case SettingsTab::Evolutionary:checkHover(evoData);         break;
                case SettingsTab::Debug:       checkHover(debugData);       break;
            }
        }
        else if (event.type == sf::Event::MouseWheelScrolled) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(game.window);
            if (hoveredTabElement && !(hoveredTabElement->isBool || hoveredTabElement->isDropdown)) {
                float stepVal = getScrollStep(hoveredTabElement->numericValue);
                if (event.mouseWheelScroll.delta > 0) {
                    hoveredTabElement->numericValue += stepVal;
                } else if (event.mouseWheelScroll.delta < 0) {
                    hoveredTabElement->numericValue -= stepVal;
                }
                if (hoveredTabElement->numericValue < hoveredTabElement->minVal) {
                    hoveredTabElement->numericValue = hoveredTabElement->minVal;
                }
                if (hoveredTabElement->numericValue > hoveredTabElement->maxVal) {
                    hoveredTabElement->numericValue = hoveredTabElement->maxVal;
                }
                if (hoveredTabElement->isFloat) {
                    hoveredTabElement->value.setString(
                            floatToStringTrimZero(hoveredTabElement->numericValue)
                    );
                } else {
                    hoveredTabElement->value.setString(
                            std::to_string(static_cast<int>(hoveredTabElement->numericValue))
                    );
                }
            }
        }
        else {
            auto handleDropdowns = [&](TabData& tabData){
                if (tabData.dropdown)  tabData.dropdown->handleEvent(event, game.window);
                if (tabData.dropdown2) tabData.dropdown2->handleEvent(event, game.window);
            };

            switch (currentTab) {
                case SettingsTab::General:     handleDropdowns(generalData);     break;
                case SettingsTab::Graphics:    handleDropdowns(graphicsData);    break;
                case SettingsTab::Debug:       handleDropdowns(debugData);       break;
                case SettingsTab::Physics:     break;
                case SettingsTab::AI:          break;
                case SettingsTab::Evolutionary:break;
            }
        }
    }
}

void SettingsState::update(Game& game) {
}

void SettingsState::render(Game& game) {
    game.window.clear(sf::Color(20, 20, 20));
    drawTabs(game.window);
    game.window.draw(panel);

    switch (currentTab) {
        case SettingsTab::General:     drawGeneral(game.window);     break;
        case SettingsTab::Graphics:    drawGraphics(game.window);    break;
        case SettingsTab::Physics:     drawPhysics(game.window);     break;
        case SettingsTab::AI:          drawAI(game.window);          break;
        case SettingsTab::Evolutionary:drawEvolutionary(game.window);break;
        case SettingsTab::Debug:       drawDebug(game.window);       break;
    }

    if (hoverSave) saveButton.setFillColor(sf::Color::Yellow);
    else           saveButton.setFillColor(sf::Color::White);

    if (hoverBack) backButton.setFillColor(sf::Color::Yellow);
    else           backButton.setFillColor(sf::Color::White);

    game.window.draw(saveButton);
    game.window.draw(backButton);
}

void SettingsState::applyChanges(Game& game) {
    if (!generalData.elements.empty()) {
        bool aiMode = (generalData.elements[0].value.getString() == "On");
        VariableManager::setAiMode(aiMode);
    }
    if (generalData.dropdown) {
        VariableManager::setUnits(generalData.dropdown->getSelectedItem());
    }

    if (graphicsData.elements.size() >= 2) {
        bool vsyncOn = (graphicsData.elements[0].value.getString() == "On");
        VariableManager::setVSync(vsyncOn);
        float fpsF = graphicsData.elements[1].numericValue;
        int fpsLim = static_cast<int>(fpsF);
        if (vsyncOn) fpsLim = 120;
        VariableManager::setFpsLimit(fpsLim);
    }
    if (graphicsData.dropdown) {
        VariableManager::setDisplayMode(graphicsData.dropdown->getSelectedItem());
    }
    if (graphicsData.dropdown2) {
        VariableManager::setResolution(graphicsData.dropdown2->getSelectedItem());
    }

    if (physicsData.elements.size() >= 5) {
        VariableManager::setMaxAccelerationConstant(         physicsData.elements[0].numericValue );
        VariableManager::setMaxAngularAccelerationConstant(   physicsData.elements[1].numericValue );
        VariableManager::setMaxSpeed(                         physicsData.elements[2].numericValue );
        VariableManager::setRotationalSpeedMultiplier(        physicsData.elements[3].numericValue );
        VariableManager::setAngularDampingMultiplier(         physicsData.elements[4].numericValue );
    }

    if (aiData.elements.size() >= 5) {
        VariableManager::setRayAmount( static_cast<int>(aiData.elements[0].numericValue) );
        VariableManager::setAiFov( aiData.elements[1].numericValue );
        VariableManager::setNetworksAmount( static_cast<int>(aiData.elements[2].numericValue) );
        VariableManager::setAiInitialRandomValuesMax( aiData.elements[3].numericValue );
        bool uniform = (aiData.elements[4].value.getString() == "On");
        VariableManager::setAiInitialRandomValuesUniform(uniform);
    }

    if (evoData.elements.size() >= 7) {
        VariableManager::setAiWinners(                static_cast<int>( evoData.elements[0].numericValue ) );
        VariableManager::setCheckpointPoints(         evoData.elements[1].numericValue );
        VariableManager::setRotationPenalty(          evoData.elements[2].numericValue );
        VariableManager::setCheckpointRadius(         evoData.elements[3].numericValue );
        VariableManager::setBackwardsMovementPenalty( evoData.elements[4].numericValue );
        VariableManager::setRestartOnDeadPercentage(  evoData.elements[5].numericValue );
        VariableManager::setMutationIndex(            evoData.elements[6].numericValue );
    }

    if (debugData.elements.size() >= 5) {
        bool fpsC      = (debugData.elements[0].value.getString() == "On");
        bool showCol   = (debugData.elements[1].value.getString() == "On");
        bool showCheck = (debugData.elements[2].value.getString() == "On");
        bool showRays  = (debugData.elements[3].value.getString() == "On");
        float carsPerc = debugData.elements[4].numericValue;

        VariableManager::setFpsCounter(fpsC);
        VariableManager::setShowColliders(showCol);
        VariableManager::setShowCheckpoints(showCheck);
        VariableManager::setShowRays(showRays);
        VariableManager::setShowCarsPercentage(carsPerc);
    }
    if (debugData.dropdown) {
        VariableManager::setMetricsMode(debugData.dropdown->getSelectedItem());
    }

    VariableManager::saveToJson(VariableManager::getPathToConfig());

    std::string resStr = VariableManager::getResolution();
    int width  = 1920;
    int height = 1080;
    if (resStr.find('x') != std::string::npos) {
        width = std::stoi(resStr.substr(0, resStr.find('x')));
        height= std::stoi(resStr.substr(resStr.find('x') + 1));
    }

    sf::Uint32 style;
    if (VariableManager::getDisplayMode() == "fullscreen") style = sf::Style::Fullscreen;
    else if (VariableManager::getDisplayMode() == "windowed") style = sf::Style::Default;
    else style = sf::Style::None;

    game.window.create(sf::VideoMode(width, height), "IntelliDrive", style);
    game.window.setVerticalSyncEnabled(VariableManager::getVSync());
    game.window.setFramerateLimit(VariableManager::getFpsLimit());
}
