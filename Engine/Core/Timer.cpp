#include "Timer.h"

void Timer::Update() {

    auto now = std::chrono::high_resolution_clock::now();

    m_deltaTime = std::chrono::duration<float>(now - m_lastTime).count();

    m_totalTime += m_deltaTime;

    m_lastTime = now;

}

