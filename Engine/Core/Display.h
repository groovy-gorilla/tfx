#pragma once
#include <string>
#include <vector>
#include <SDL3/SDL.h>

struct Mode {
    int width;
    int height;
    std::string ratio;
};

struct DisplayInfo {
    SDL_DisplayID id;
    std::string name;
    int modeCount;
    std::vector<Mode> displayModes;
};

class Display {
public:
    void Initialize();

    const DisplayInfo& GetPrimaryDisplay() const;                   // Pobiera główny monitor

    std::vector<Mode> GetDisplayModes() const;                      // Pobiera tryby monitora
    const SDL_DisplayMode* GetDisplayNativeMode();                  // Pobiera tryb natywny monitora

    float GetScaling();                                             // Pobiera skalowanie

private:
    DisplayInfo m_displayInfo{};
    SDL_DisplayMode* m_nativeMode = nullptr;

};
