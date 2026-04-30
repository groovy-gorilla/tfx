#pragma once
#include <string>
#include <vector>
#include <SDL3/SDL.h>

struct Mode {
    int width;
    int height;
    float refreshRate;
};

struct DisplayInfo {
    Uint32 id;
    std::string name;
    int modeCount;
    std::vector<Mode> displayModes;
};

class Display {
public:
    void Initialize();

    const std::vector<DisplayInfo>& GetDisplays() const;
    int GetDisplayCount();
    void SetCurrentDisplay(int index);
    const DisplayInfo& GetCurrentDisplay() const;
    const std::vector<Mode>& GetDisplayModes(int displayIndex) const;
    Mode GetDisplayNativeMode();
    void SetCurrentMode(int modeIndex);
    Mode GetCurrentMode();
    float GetScaling();

private:
    std::vector<DisplayInfo> m_displays;
    int m_displayCount = 0;
    float m_displayScaling = 1.0f;
    int m_currentDisplayIndex = 0;
    int m_currentModeIndex = 0;

};
