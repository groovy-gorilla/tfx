#pragma once
#include <string>
#include <vector>
#include <SDL3/SDL.h>

struct DisplayInfo {
    SDL_DisplayID id;
    std::string name;
    int modeCount;
    SDL_DisplayMode** displayModes;
};

class Display {
public:
    void Initialize();

    const std::vector<DisplayInfo>& GetDisplays() const;            // Pobiera listę monitorów
    int GetDisplayCount();                                          // Pobiera liczbę monitorów

    void SetCurrentDisplay(int index);                              // Ustawia aktualny monitor
    const DisplayInfo& GetCurrentDisplay() const;                   // Pobiera aktualny monitor

    SDL_DisplayMode** GetDisplayModes(int displayIndex) const;      // Pobiera tryby monitora
    const SDL_DisplayMode* GetDisplayNativeMode();                  // Pobiera tryb natywny monitora

    void SetCurrentMode(int modeIndex);                             // Ustawia aktualny tryb
    const SDL_DisplayMode* GetCurrentMode();                        // Pobiera aktualny tryb

    float GetScaling();                                             // Pobiera skalowanie

private:
    std::vector<DisplayInfo> m_displays;

    int m_displayCount = 0;
    int m_currentDisplayIndex = 0;
    int m_currentDisplayModeIndex = 0;


};
