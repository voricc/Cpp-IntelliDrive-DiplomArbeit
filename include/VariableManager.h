//
// Created by Tobias on 14.01.2025.
//

#ifndef INTELLIDRIVE_VARIABLEMANAGER_H
#define INTELLIDRIVE_VARIABLEMANAGER_H

// Includes
#include "Car.h"
#include "../vendors/json/json.hpp"
#include "../vendors/ai/NeuralNetwork/NeuralNetwork.h"
#include <unordered_map>
#include <tile.h>
#include <fstream>
#include <vector>

// Definitions
#define OUTPUT_NEURONS 2
#define PI 3.14159265f

class VariableManager{
private:

    // Physics
    static float MAX_ACCELERATION_CONSTANT; // Units: pixels per second squared
    static float MAX_ANGULAR_ACCELERATION_CONSTANT; // Units: degrees per second squared
    static float MAX_SPEED; // Units: pixels per second
    static float ROTATIONAL_SPEED_MULTIPLIER;
    static float ANGULAR_DAMPING_MULTIPLIER;

    // Car
    static int SELECTED_CAR_INDEX;
    //static std::vector<carData> cars;

    // World
    static float TILE_SIZE;

    // Resources
    static std::string FONT_RUBIK_REGULAR;

    static std::unordered_map<std::string, sf::Font> FONTS;
    static std::unordered_map<std::string, sf::Texture> TEXTURES;
    static std::vector<Tile> TILES;

    // AI
    static int RAY_AMOUNT;
    static float AI_FOV;
    static int NETWORKS_AMOUNT;
    static std::vector<int> AI_TOPOLOGY;
    static float AI_INITIAL_RANDOM_VALUES_MAX;
    static bool AI_INITIAL_RANDOM_VALUES_UNIFORM;
    static std::vector<Utility::Activations> AI_ACTIVATIONS;

    // Evolutionary Algorithm
    static int AI_WINNERS;
    static float CHECKPOINT_POINTS;
    static float CHECKPOINT_RADIUS;
    static float ROTATION_PENALTY;
    static float BACKWARDS_MOVEMENT_PENALTY;
    static float RESTART_ON_DEAD_PERCENTAGE;
    static float MUTATION_INDEX;

    // DEBUG - RENDER
    static bool SHOW_COLLIDERS;
    static bool SHOW_CHECKPOINTS;
    static bool SHOW_RAYS;
    static float SHOW_CARS_PERCENTAGE;
public:

    /// Load and Save functions
    static void VariableManager::loadFromJson(const std::string& filePath);
    static void VariableManager::saveToJson(const std::string& filePath);

    /// Getter and Setter functions

    // Math
    static float getPI() { return PI; }

    // Physics
    static float getMaxAccelerationConstant() { return MAX_ACCELERATION_CONSTANT; }
    static void setMaxAccelerationConstant(float value) { MAX_ACCELERATION_CONSTANT = value; }

    static float getMaxAngularAccelerationConstant() { return MAX_ANGULAR_ACCELERATION_CONSTANT; }
    static void setMaxAngularAccelerationConstant(float value) { MAX_ANGULAR_ACCELERATION_CONSTANT = value; }

    static float getMaxSpeed() { return MAX_SPEED; }
    static void setMaxSpeed(float value) { MAX_SPEED = value; }

    static float getRotationalSpeedMultiplier() { return ROTATIONAL_SPEED_MULTIPLIER; }
    static void setRotationalSpeedMultiplier(float value) { ROTATIONAL_SPEED_MULTIPLIER = value; }

    static float getAngularDampingMultiplier() { return ANGULAR_DAMPING_MULTIPLIER; }
    static void setAngularDampingMultiplier(float value) { ANGULAR_DAMPING_MULTIPLIER = value; }

    // Car
    static int getSelectedCarIndex() { return SELECTED_CAR_INDEX; }
    static void setSelectedCarIndex(int value) { SELECTED_CAR_INDEX = value; }

    // World
    static float getTileSize() { return TILE_SIZE; }
    static void setTileSize(float value) { TILE_SIZE = value; }

    // Resources
    static std::string getFontRubikRegular() { return FONT_RUBIK_REGULAR; }
    static void setFontRubikRegular(const std::string& value) { FONT_RUBIK_REGULAR = value; }

    // AI
    static int getRayAmount() { return RAY_AMOUNT; }
    static void setRayAmount(int value) { RAY_AMOUNT = value; }

    static float getAiFov() { return AI_FOV; }
    static void setAiFov(float value) { AI_FOV = value; }

    static int getNetworksAmount() { return NETWORKS_AMOUNT; }
    static void setNetworksAmount(int value) { NETWORKS_AMOUNT = value; }

    static std::vector<int> &getAiTopology() { return AI_TOPOLOGY; }
    static void setAiTopology(std::vector<int> &value) { AI_TOPOLOGY = value; }

    static std::vector<Utility::Activations> &getAiActivations() { return AI_ACTIVATIONS; }
    static void setAiActivations(std::vector<Utility::Activations> &value) { AI_ACTIVATIONS = value; }

    static float getAiInitialRandomValuesMax() { return AI_INITIAL_RANDOM_VALUES_MAX; }
    static void setAiInitialRandomValuesMax(float value) { AI_INITIAL_RANDOM_VALUES_MAX = value; }

    static bool getAiInitialRandomValuesUniform() { return AI_INITIAL_RANDOM_VALUES_UNIFORM; }
    static void setAiInitialRandomValuesUniform(bool value) { AI_INITIAL_RANDOM_VALUES_UNIFORM = value; }

    // Evolutionary Algorithm
    static  int getAiWinners() { return AI_WINNERS; }
    static void setAiWinners(int value) { AI_WINNERS = value; }

    static float getCheckpointPoints() { return CHECKPOINT_POINTS; }
    static void setCheckpointPoints(float value) { CHECKPOINT_POINTS = value; }

    static float getCheckpointRadius() { return CHECKPOINT_RADIUS; }
    static void setCheckpointRadius(float value) { CHECKPOINT_RADIUS = value; }

    static float getRotationPenalty() { return ROTATION_PENALTY; }
    static void setRotationPenalty(float value) { ROTATION_PENALTY = value; }

    static float getBackwardsMovementPenalty() { return BACKWARDS_MOVEMENT_PENALTY; }
    static void setBackwardsMovementPenalty(float value) { BACKWARDS_MOVEMENT_PENALTY = value; }

    static float getRestartOnDeadPercentage() { return RESTART_ON_DEAD_PERCENTAGE; }
    static void setRestartOnDeadPercentage(float value) { RESTART_ON_DEAD_PERCENTAGE = value; }

    static float getMutationIndex() { return MUTATION_INDEX; }
    static void setMutationIndex(float value) { MUTATION_INDEX = value; }

    // DEBUG - RENDER
    static bool getShowColliders() { return SHOW_COLLIDERS; }
    static void setShowColliders(bool value) { SHOW_COLLIDERS = value; }

    static bool getShowCheckpoints() { return SHOW_CHECKPOINTS; }
    static void setShowCheckpoints(bool value) { SHOW_CHECKPOINTS = value; }

    static bool getShowRays() { return SHOW_RAYS; }
    static void setShowRays(bool value) { SHOW_RAYS = value; }

    static float getShowCarsPercentage() { return SHOW_CARS_PERCENTAGE; }
    static void setShowCarsPercentage(float value) { SHOW_CARS_PERCENTAGE = value; }

};

#endif //INTELLIDRIVE_VARIABLEMANAGER_H