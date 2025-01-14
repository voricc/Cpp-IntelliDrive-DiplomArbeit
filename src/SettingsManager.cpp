//
// Created by Devrim on 08/12/2024.
//

// https://github.com/nlohmann/json IMPORTANT DONT DELETE

#include "SettingsManager.h"


bool SettingsManager::ai_mode = false;
std::string SettingsManager::units = "Metric";
std::string SettingsManager::display_mode = "fullscreen";
std::string SettingsManager::resolution = "1920x1080";
bool SettingsManager::vsync = false;
int SettingsManager::fps_limit = 120;
bool SettingsManager::debug_mode = false;
bool SettingsManager::fps_counter = false;
std::string SettingsManager::metrics_mode = "simple";

using json = nlohmann::json;

SettingsManager::SettingsManager() { }

SettingsManager& SettingsManager::getInstance() {
    static SettingsManager instance;
    return instance;
}

void SettingsManager::load(const std::string& path) {
    std::ifstream in(path);
    if(!in.good()) return;
    json j; in >> j;
    if(j.contains("ai_mode")) ai_mode = j["ai_mode"].get<bool>();
    if(j.contains("units")) units = j["units"].get<std::string>();
    if(j.contains("display_mode")) display_mode = j["display_mode"].get<std::string>();
    if(j.contains("resolution")) resolution = j["resolution"].get<std::string>();
    if(j.contains("vsync")) vsync = j["vsync"].get<bool>();
    if(j.contains("fps_limit")) fps_limit = j["fps_limit"].get<int>();
    if(j.contains("debug_mode")) debug_mode = j["debug_mode"].get<bool>();
    if(j.contains("fps_counter")) fps_counter = j["fps_counter"].get<bool>();
    if(j.contains("metrics_mode")) metrics_mode = j["metrics_mode"].get<std::string>();
}

void SettingsManager::save(const std::string& path) {
    json j;
    j["ai_mode"] = ai_mode;
    j["units"] = units;
    j["display_mode"] = display_mode;
    j["resolution"] = resolution;
    j["vsync"] = vsync;
    j["fps_limit"] = fps_limit;
    j["debug_mode"] = debug_mode;
    j["fps_counter"] = fps_counter;
    j["metrics_mode"] = metrics_mode;
    std::ofstream out(path);
    out << j.dump(4);
}

bool SettingsManager::getAIMode() { return ai_mode; }
void SettingsManager::setAIMode(bool v) { ai_mode = v; }

std::string SettingsManager::getUnits() { return units; }
void SettingsManager::setUnits(const std::string& u) { units = u; }

std::string SettingsManager::getDisplayMode() { return display_mode; }
void SettingsManager::setDisplayMode(const std::string& m) { display_mode = m; }

std::string SettingsManager::getResolution() { return resolution; }
void SettingsManager::setResolution(const std::string& r) { resolution = r; }

bool SettingsManager::getVSync() { return vsync; }
void SettingsManager::setVSync(bool v) { vsync = v; }

int SettingsManager::getFPSLimit() { return fps_limit; }
void SettingsManager::setFPSLimit(int f) { fps_limit = f; }

bool SettingsManager::getDebugMode() { return debug_mode; }
void SettingsManager::setDebugMode(bool v) { debug_mode = v; }

bool SettingsManager::getFPSCounter() { return fps_counter; }
void SettingsManager::setFPSCounter(bool v) { fps_counter = v; }

std::string SettingsManager::getMetricsMode() { return metrics_mode; }
void SettingsManager::setMetricsMode(const std::string& m) { metrics_mode = m; }