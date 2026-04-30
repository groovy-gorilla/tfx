#include "Display.h"

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
        SDL_DisplayMode** dm = SDL_GetFullscreenDisplayModes(displays[i], &di.modeCount);
        di.displayModes.resize(di.modeCount);
        for (int j = 0; j < di.modeCount; j++) {
            di.displayModes[j].width = dm[j]->w;
            di.displayModes[j].height = dm[j]->h;
            di.displayModes[j].refreshRate = dm[j]->refresh_rate;
        }
        m_displays[i] = di;
        SDL_free(dm);
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

const std::vector<Mode>& Display::GetDisplayModes(int displayIndex) const {
    return m_displays[displayIndex].displayModes;
}

Mode Display::GetDisplayNativeMode() {
    return m_displays[m_currentDisplayIndex].displayModes[0];
}

void Display::SetCurrentMode(int modeIndex) {
    m_currentModeIndex = modeIndex;
}

Mode Display::GetCurrentMode() {
    return m_displays[m_currentDisplayIndex].displayModes[m_currentModeIndex];
}

float Display::GetScaling() {
    const SDL_DisplayMode* mcurr = SDL_GetCurrentDisplayMode(m_displays[m_currentDisplayIndex].id);
    if (!mcurr) { return 1.0f; }
    return static_cast<float>(m_displays[m_currentDisplayIndex].displayModes[m_currentModeIndex].width) / mcurr->w;
}

