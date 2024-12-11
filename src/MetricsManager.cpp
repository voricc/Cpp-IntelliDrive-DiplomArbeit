//
// Created by Devrim on 08/12/2024.
//

#include "MetricsManager.h"

MetricsManager::MetricsManager() {
    lastFrame = std::chrono::high_resolution_clock::now();
    fps = 0.f;
    frameTime = 0.f;
}

MetricsManager& MetricsManager::getInstance() {
    static MetricsManager instance;
    return instance;
}

void MetricsManager::frameStart() {
    lastFrame = std::chrono::high_resolution_clock::now();
}

void MetricsManager::frameEnd() {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> diff = now - lastFrame;
    frameTime = diff.count() * 1000.0f;
    fps = 1.0f / diff.count();
}

float MetricsManager::getFPS() const {
    return fps;
}

float MetricsManager::getFrameTime() const {
    return frameTime;
}

float MetricsManager::getCPUUsage() const {
    return 0.0f;
}

float MetricsManager::getGPUUsage() const {
    return 0.0f;
}

float MetricsManager::getMemoryUsage() const {
    return 0.0f;
}