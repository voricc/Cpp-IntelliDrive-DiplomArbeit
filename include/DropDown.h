//
// Created by Devrim on 08/12/2024.
//

#ifndef DROPDOWN_H
#define DROPDOWN_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Dropdown {
public:
    Dropdown(const sf::Font& font, const std::vector<std::string>& items, float x, float y, float width, float height);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    std::string getSelectedItem() const;
    void setSelectedItem(const std::string& item);

private:
    sf::RectangleShape box;
    sf::Text selectedText;
    std::vector<sf::Text> itemTexts;
    bool expanded;
    float itemHeight;
};

#endif