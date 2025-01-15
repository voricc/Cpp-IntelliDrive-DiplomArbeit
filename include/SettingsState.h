#ifndef SETTINGSSTATE_H
#define SETTINGSSTATE_H

#include "State.h"
#include "Game.h"
#include "MenuState.h"
#include "Dropdown.h"
#include <SFML/Graphics.hpp>
#include <vector>

enum class SettingsTab {
    General,
    Graphics,
    Physics,
    AI,
    Evolutionary,
    Debug
};

/// Represents one “setting” displayed on-screen:
struct TabElement {
    sf::Text label;        // e.g. "Max Speed"
    sf::Text value;        // e.g. "450"
    float    numericValue  = 0.f;
    float    step          = 1.f;
    float    minVal        = 0.f;
    float    maxVal        = 999999.f;
    bool     isFloat       = true;  // float vs. int
    bool     isBool        = false; // toggles "On"/"Off" if true
    bool     isDropdown    = false; // purely a label for an associated dropdown
    bool     incrementOnClick = true;
};

/// A tab can store multiple elements + optional dropdowns
struct TabData {
    std::vector<TabElement> elements;
    Dropdown* dropdown  = nullptr;
    Dropdown* dropdown2 = nullptr;
};

class SettingsState : public State {
public:
    SettingsState();
    void handleInput(Game& game) override;
    void update(Game& game) override;
    void render(Game& game) override;

private:
    SettingsTab currentTab;
    sf::Font font;
    sf::RectangleShape panel;

    sf::Text generalTabText;
    sf::Text graphicsTabText;
    sf::Text physicsTabText;
    sf::Text aiTabText;
    sf::Text evoTabText;
    sf::Text debugTabText;

    sf::Text saveButton;
    sf::Text backButton;
    bool hoverSave;
    bool hoverBack;

    // Tab data for each category:
    TabData generalData;
    TabData graphicsData;
    TabData physicsData;
    TabData aiData;
    TabData evoData;
    TabData debugData;

    Dropdown* unitsDropdown;
    Dropdown* displayModeDropdown;
    Dropdown* resolutionDropdown;
    Dropdown* metricsDropdown;

    static constexpr float LABEL_X          = 200.f;
    static constexpr float VALUE_X          = 550.f;
    static constexpr float DROPDOWN_X       = 550.f;
    static constexpr float DROPDOWN_WIDTH   = 250.f;
    static constexpr float DROPDOWN_HEIGHT  = 40.f;
    static constexpr float START_Y          = 200.f;
    static constexpr float GAP_Y            = 50.f;

    // Helper methods
    void setupText(sf::Text& txt, const std::string& str, float x, float y);
    std::string floatToStringTrimZero(float val);
    void loadUI();
    void switchTab(SettingsTab t);
    void drawTabs(sf::RenderWindow& window);

    void drawTabData(sf::RenderWindow& window, TabData& data);
    void drawGeneral(sf::RenderWindow& window);
    void drawGraphics(sf::RenderWindow& window);
    void drawPhysics(sf::RenderWindow& window);
    void drawAI(sf::RenderWindow& window);
    void drawEvolutionary(sf::RenderWindow& window);
    void drawDebug(sf::RenderWindow& window);

    void handleTabElementClick(TabElement& elem, sf::Vector2i mousePos, bool isRightClick);
    void applyChanges(Game& game);
};

#endif
