//
// Created by Voric on 11/08/2024.
//

#include "SettingsManager.h"
#include "../include/Game.h"
#include "../include/MenuState.h"

int main() {
    SettingsManager::getInstance().load("resources/config/config.json");
    Game game;
    game.run();
    return 0;
}
