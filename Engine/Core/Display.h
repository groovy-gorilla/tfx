#pragma once

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

    [[nodiscard]] const DisplayInfo& GetPrimaryDisplay() const;                   // Pobiera główny monitor

    [[nodiscard]] std::vector<Mode> GetDisplayModes() const;                      // Pobiera tryby monitora
    const SDL_DisplayMode* GetDisplayNativeMode();                  // Pobiera tryb natywny monitora
    const SDL_DisplayMode* GetCurrentDisplayMode();                 // Pobiera aktualny tryb monitora

    float GetScaling();                                             // Pobiera skalowanie

private:
    DisplayInfo m_curretnDisplayInfo{};
    SDL_DisplayMode* m_nativeMode = nullptr;

};
