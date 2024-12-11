//
// Created by Voric on 08/12/2024
// []

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <string>

class SettingsManager {
public:
    static SettingsManager& getInstance();
    static void load(const std::string& path);
    static void save(const std::string& path);

    static bool getAIMode();
    static void setAIMode(bool v);

    static std::string getUnits();
    static void setUnits(const std::string& u);

    static std::string getDisplayMode();
    static void setDisplayMode(const std::string& m);

    static std::string getResolution();
    static void setResolution(const std::string& r);

    static bool getVSync();
    static void setVSync(bool v);

    static int getFPSLimit();
    static void setFPSLimit(int f);

    static bool getDebugMode();
    static void setDebugMode(bool v);

    static bool getFPSCounter();
    static void setFPSCounter(bool v);

    static std::string getMetricsMode();
    static void setMetricsMode(const std::string& m);

private:
    SettingsManager();
    static bool ai_mode;
    static std::string units;
    static std::string display_mode;
    static std::string resolution;
    static bool vsync;
    static int fps_limit;
    static bool debug_mode;
    static bool fps_counter;
    static std::string metrics_mode;
};

#endif