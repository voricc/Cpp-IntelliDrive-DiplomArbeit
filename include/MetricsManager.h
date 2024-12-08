//
// Created by Devrim on 08/12/2024.
//

#ifndef METRICSMANAGER_H
#define METRICSMANAGER_H

#include <chrono>

class MetricsManager {
public:
    static MetricsManager& getInstance();
    void frameStart();
    void frameEnd();
    float getFPS() const;
    float getFrameTime() const;
    float getCPUUsage() const;
    float getGPUUsage() const;
    float getMemoryUsage() const;

private:
    MetricsManager();
    std::chrono::high_resolution_clock::time_point lastFrame;
    float fps;
    float frameTime;
};

#endif