//
// Created by Devrim on 08/12/2024.
//

#include "Dropdown.h"

Dropdown::Dropdown(const sf::Font& font, const std::vector<std::string>& items, float x, float y, float width, float height) {
    box.setPosition(x, y);
    box.setSize({width, height});
    box.setFillColor(sf::Color(50,50,50));
    box.setOutlineColor(sf::Color::White);
    box.setOutlineThickness(2);
    expanded = false;
    itemHeight = height;
    if(!items.empty()) {
        selectedText.setFont(font);
        selectedText.setString(items[0]);
        selectedText.setCharacterSize(24);
        selectedText.setFillColor(sf::Color::White);
        sf::FloatRect b = selectedText.getLocalBounds();
        selectedText.setOrigin(b.left, b.top);
        selectedText.setPosition(x+10, y+(height/2 - b.height/2));
        for(size_t i=0; i<items.size(); i++) {
            sf::Text t;
            t.setFont(font);
            t.setString(items[i]);
            t.setCharacterSize(24);
            t.setFillColor(sf::Color::White);
            sf::FloatRect tb = t.getLocalBounds();
            t.setPosition(x+10, y+height*(i+1)+(height/2 - tb.height/2));
            itemTexts.push_back(t);
        }
    }
}

void Dropdown::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if(box.getGlobalBounds().contains((float)mousePos.x,(float)mousePos.y)) {
            expanded = !expanded;
        } else if(expanded) {
            for(size_t i=0; i<itemTexts.size(); i++) {
                if(sf::FloatRect(itemTexts[i].getPosition().x, itemTexts[i].getPosition().y, box.getSize().x, box.getSize().y).contains((float)mousePos.x,(float)mousePos.y)) {
                    selectedText.setString(itemTexts[i].getString());
                    expanded = false;
                    break;
                }
            }
        }
    }
}

void Dropdown::draw(sf::RenderWindow& window) {
    window.draw(box);
    window.draw(selectedText);
    if(expanded) {
        sf::RectangleShape bg;
        bg.setPosition(box.getPosition().x, box.getPosition().y+box.getSize().y);
        bg.setSize({box.getSize().x, box.getSize().y*(float)itemTexts.size()});
        bg.setFillColor(sf::Color(70,70,70));
        bg.setOutlineThickness(2);
        bg.setOutlineColor(sf::Color::White);
        window.draw(bg);
        for(auto& t : itemTexts) {
            window.draw(t);
        }
    }
}

std::string Dropdown::getSelectedItem() const {
    return selectedText.getString();
}

void Dropdown::setSelectedItem(const std::string& item) {
    selectedText.setString(item);
}