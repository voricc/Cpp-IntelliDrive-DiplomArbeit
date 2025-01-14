//
// Created by Tobias on 14.01.2025.
//

#include "VariableManager.h"

// Physics
float VariableManager::MAX_ACCELERATION_CONSTANT = 800.0f; // Units: pixels per second squared
float VariableManager::MAX_ANGULAR_ACCELERATION_CONSTANT = 200.0f; // Units: degrees per second squared
float VariableManager::MAX_SPEED = 450.0f; // Units: pixels per second
float VariableManager::ROTATIONAL_SPEED_MULTIPLIER = 0.00425;
float VariableManager::ANGULAR_DAMPING_MULTIPLIER = 0.99;

// Car
int VariableManager::SELECTED_CAR_INDEX = 0;

// World
float VariableManager::TILE_SIZE = 64;

// Resources
std::string VariableManager::FONT_RUBIK_REGULAR = "resources/Fonts/Rubik-Regular.ttf";

// AI
int VariableManager::RAY_AMOUNT = 5;
float VariableManager::AI_FOV = 270.0f;
int VariableManager::NETWORKS_AMOUNT = 5000;
float VariableManager::AI_INITIAL_RANDOM_VALUES_MAX = 1.6f;
bool VariableManager::AI_INITIAL_RANDOM_VALUES_UNIFORM = true;
std::vector<int> VariableManager::AI_TOPOLOGY = {RAY_AMOUNT, 4, OUTPUT_NEURONS};
std::vector<Utility::Activations> VariableManager::AI_ACTIVATIONS = {
        Utility::Activations::Tanh,
        Utility::Activations::Tanh
};

// Evolutionary Algorithm
int VariableManager::AI_WINNERS = 10;
float VariableManager::CHECKPOINT_POINTS = 25;
float VariableManager::ROTATION_PENALTY;
float VariableManager::CHECKPOINT_RADIUS = 140.0f;
float VariableManager::BACKWARDS_MOVEMENT_PENALTY;
float VariableManager::RESTART_ON_DEAD_PERCENTAGE = 0.995;
float VariableManager::MUTATION_INDEX = 0.04f;

// DEBUG - RENDER
bool VariableManager::SHOW_COLLIDERS = false;
bool VariableManager::SHOW_CHECKPOINTS = false;
bool VariableManager::SHOW_RAYS = false;
float VariableManager::SHOW_CARS_PERCENTAGE = 1.0f;

// Save function
void VariableManager::saveToJson(const std::string& filePath) {
    nlohmann::json jsonData;
    // Physics
    jsonData["MAX_ACCELERATION_CONSTANT"] = MAX_ACCELERATION_CONSTANT;
    jsonData["MAX_ANGULAR_ACCELERATION_CONSTANT"] = MAX_ANGULAR_ACCELERATION_CONSTANT;
    jsonData["MAX_SPEED"] = MAX_SPEED;
    jsonData["ROTATIONAL_SPEED_MULTIPLIER"] = ROTATIONAL_SPEED_MULTIPLIER;
    jsonData["ANGULAR_DAMPING_MULTIPLIER"] = ANGULAR_DAMPING_MULTIPLIER;
    jsonData["SELECTED_CAR_INDEX"] = SELECTED_CAR_INDEX;

    // World
    jsonData["TILE_SIZE"] = TILE_SIZE;

    // Resources
    jsonData["FONT_RUBIK_REGULAR"] = FONT_RUBIK_REGULAR;

    // AI
    jsonData["RAY_AMOUNT"] = RAY_AMOUNT;
    jsonData["AI_FOV"] = AI_FOV;
    jsonData["NETWORKS_AMOUNT"] = NETWORKS_AMOUNT;
    jsonData["AI_TOPOLOGY"] = AI_TOPOLOGY;
    jsonData["AI_INITIAL_RANDOM_VALUES_MAX"] = AI_INITIAL_RANDOM_VALUES_MAX;
    jsonData["AI_INITIAL_RANDOM_VALUES_UNIFORM"] = AI_INITIAL_RANDOM_VALUES_UNIFORM;

    // Evolutionary Algorithm
    jsonData["AI_WINNERS"] = AI_WINNERS;
    jsonData["CHECKPOINT_POINTS"] = CHECKPOINT_POINTS;
    jsonData["ROTATION_PENALTY"] = ROTATION_PENALTY;
    jsonData["BACKWARDS_MOVEMENT_PENALTY"] = BACKWARDS_MOVEMENT_PENALTY;
    jsonData["RESTART_ON_DEAD_PERCENTAGE"] = RESTART_ON_DEAD_PERCENTAGE;
    jsonData["MUTATION_INDEX"] = MUTATION_INDEX;

    // Debug - Render
    jsonData["SHOW_COLLIDERS"] = SHOW_COLLIDERS;
    jsonData["SHOW_CHECKPOINTS"] = SHOW_CHECKPOINTS;
    jsonData["SHOW_RAYS"] = SHOW_RAYS;
    jsonData["SHOW_CARS_PERCENTAGE"] = SHOW_CARS_PERCENTAGE;

    std::ofstream file(filePath);
    file << jsonData.dump(4);
}

// Load function
void VariableManager::loadFromJson(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) return;

    nlohmann::json jsonData;
    file >> jsonData;

    // Physics
    MAX_ACCELERATION_CONSTANT = jsonData.value("MAX_ACCELERATION_CONSTANT", MAX_ACCELERATION_CONSTANT);
    MAX_ANGULAR_ACCELERATION_CONSTANT = jsonData.value("MAX_ANGULAR_ACCELERATION_CONSTANT", MAX_ANGULAR_ACCELERATION_CONSTANT);
    MAX_SPEED = jsonData.value("MAX_SPEED", MAX_SPEED);
    ROTATIONAL_SPEED_MULTIPLIER = jsonData.value("ROTATIONAL_SPEED_MULTIPLIER", ROTATIONAL_SPEED_MULTIPLIER);
    ANGULAR_DAMPING_MULTIPLIER = jsonData.value("ANGULAR_DAMPING_MULTIPLIER", ANGULAR_DAMPING_MULTIPLIER);

    // Car
    SELECTED_CAR_INDEX = jsonData.value("SELECTED_CAR_INDEX", SELECTED_CAR_INDEX);

    // World
    TILE_SIZE = jsonData.value("TILE_SIZE", TILE_SIZE);

    // Resources
    FONT_RUBIK_REGULAR = jsonData.value("FONT_RUBIK_REGULAR", FONT_RUBIK_REGULAR);

    // AI
    RAY_AMOUNT = jsonData.value("RAY_AMOUNT", RAY_AMOUNT);
    AI_FOV = jsonData.value("AI_FOV", AI_FOV);
    NETWORKS_AMOUNT = jsonData.value("NETWORKS_AMOUNT", NETWORKS_AMOUNT);
    AI_TOPOLOGY = jsonData.value("AI_TOPOLOGY", AI_TOPOLOGY);
    AI_INITIAL_RANDOM_VALUES_MAX = jsonData.value("AI_INITIAL_RANDOM_VALUES_MAX", AI_INITIAL_RANDOM_VALUES_MAX);
    AI_INITIAL_RANDOM_VALUES_UNIFORM = jsonData.value("AI_INITIAL_RANDOM_VALUES_UNIFORM",AI_INITIAL_RANDOM_VALUES_UNIFORM);

    // Evolutionary Algorithm
    AI_WINNERS = jsonData.value("AI_WINNERS", AI_WINNERS);
    CHECKPOINT_POINTS = jsonData.value("CHECKPOINT_POINTS", CHECKPOINT_POINTS);
    ROTATION_PENALTY = jsonData.value("ROTATION_PENALTY", ROTATION_PENALTY);
    BACKWARDS_MOVEMENT_PENALTY = jsonData.value("BACKWARDS_MOVEMENT_PENALTY", BACKWARDS_MOVEMENT_PENALTY);
    RESTART_ON_DEAD_PERCENTAGE = jsonData.value("RESTART_ON_DEAD_PERCENTAGE", RESTART_ON_DEAD_PERCENTAGE);
    MUTATION_INDEX = jsonData.value("MUTATION_INDEX", MUTATION_INDEX);

    // Debug - Render
    SHOW_COLLIDERS = jsonData.value("SHOW_COLLIDERS", SHOW_COLLIDERS);
    SHOW_CHECKPOINTS = jsonData.value("SHOW_CHECKPOINTS", SHOW_CHECKPOINTS);
    SHOW_RAYS = jsonData.value("SHOW_RAYS", SHOW_RAYS);
    SHOW_CARS_PERCENTAGE = jsonData.value("SHOW_CARS_PERCENTAGE", SHOW_CARS_PERCENTAGE);
}