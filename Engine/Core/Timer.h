#pragma once

#include "pch.h"

class Timer {
public:
    void Update();

    [[nodiscard]] float GetDeltaTime() const { return m_deltaTime; }
    [[nodiscard]] float GetTotalTime() const { return m_totalTime; }

private:
    std::chrono::high_resolution_clock::time_point m_lastTime = std::chrono::high_resolution_clock::now();
    float m_deltaTime = 0.0f;
    float m_totalTime = 0.0f;
};