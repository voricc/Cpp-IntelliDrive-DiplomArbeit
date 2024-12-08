//
// Created by Voric on 08/12/2024
// []

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <string>

class SettingsManager {
public:
    static SettingsManager& getInstance();
    void load(const std::string& path);
    void save(const std::string& path);

    bool getAIMode() const;
    void setAIMode(bool v);

    std::string getUnits() const;
    void setUnits(const std::string& u);

    std::string getDisplayMode() const;
    void setDisplayMode(const std::string& m);

    std::string getResolution() const;
    void setResolution(const std::string& r);

    bool getVSync() const;
    void setVSync(bool v);

    int getFPSLimit() const;
    void setFPSLimit(int f);

    bool getDebugMode() const;
    void setDebugMode(bool v);

    bool getFPSCounter() const;
    void setFPSCounter(bool v);

    std::string getMetricsMode() const;
    void setMetricsMode(const std::string& m);

private:
    SettingsManager();
    bool ai_mode;
    std::string units;
    std::string display_mode;
    std::string resolution;
    bool vsync;
    int fps_limit;
    bool debug_mode;
    bool fps_counter;
    std::string metrics_mode;
};

#endif