#include "Display.h"

#include <iostream>
#include <ostream>
#include <stdexcept>

void Display::Initialize() {

    SDL_DisplayID* displays = nullptr;

    // Pobiera listę monitorów
    displays = SDL_GetDisplays(&m_displayCount);
    if (!displays) {
        throw std::runtime_error(SDL_GetError());
    }

    m_displays.resize(m_displayCount);

    // Tworzy listę informacji o monitorach
    for (int i = 0; i < m_displayCount; i++) {
        DisplayInfo di;
        di.id = displays[i];
        di.name = SDL_GetDisplayName(displays[i]);
        di.displayModes = SDL_GetFullscreenDisplayModes(displays[i], &di.modeCount);
        m_displays[i] = di;

        std::cout << "[Monitor][" << i << "]: " << m_displays[i].name << std::endl;
        for (int j = 0; j < di.modeCount; j++) {
            std::cout << m_displays[i].displayModes[j]->w << "x" << m_displays[i].displayModes[j]->h << std::endl;
        }

    }

    SDL_free(displays);

}

const std::vector<DisplayInfo>& Display::GetDisplays() const {
    return m_displays;
}

int Display::GetDisplayCount() {
    return m_displayCount;
}

void Display::SetCurrentDisplay(int index) {
    m_currentDisplayIndex = index;
}

const DisplayInfo& Display::GetCurrentDisplay() const {
    return m_displays[m_currentDisplayIndex];
}

SDL_DisplayMode** Display::GetDisplayModes(int displayIndex) const {
    return m_displays[displayIndex].displayModes;
}

const SDL_DisplayMode* Display::GetDisplayNativeMode() {
    return m_displays[m_currentDisplayIndex].displayModes[0];
}

void Display::SetCurrentMode(int modeIndex) {
    m_currentDisplayModeIndex = modeIndex;
}

const SDL_DisplayMode* Display::GetCurrentMode() {
    return m_displays[m_currentDisplayIndex].displayModes[m_currentDisplayModeIndex];
}

float Display::GetScaling() {
    const SDL_DisplayMode* mcurr = SDL_GetCurrentDisplayMode(m_displays[m_currentDisplayIndex].id);
    if (!mcurr) { return 1.0f; }
    std::cout << "Scaling: " << static_cast<float>(m_displays[m_currentDisplayIndex].displayModes[0]->w) / mcurr->w << std::endl;
    return static_cast<float>(m_displays[m_currentDisplayIndex].displayModes[0]->w) / mcurr->w;
}

