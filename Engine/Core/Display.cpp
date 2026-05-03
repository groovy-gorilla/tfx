#include "Display.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <ostream>
#include <stdexcept>

void Display::Initialize() {

    SDL_DisplayID* displays = nullptr;

    // Pobiera listę monitorów
    displays = SDL_GetDisplays(nullptr);
    if (!displays) {
        throw std::runtime_error(SDL_GetError());
    }

    // Tworzy listę informacji o monitorze
    m_displayInfo.id = displays[0];                                                                         // Wybiera główny monitor (SDL_GetPrimaryDisplay często nie działa prawidłowo)
    m_displayInfo.name = SDL_GetDisplayName(displays[0]);                                                   // Pobiera nazwę monitora

    int count = 0;
    SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(displays[0], &count);         // Pobiera tryby monitora i ich ilość
    m_displayInfo.displayModes.resize(count);
    for (int i = 0; i < m_displayInfo.displayModes.size(); i++) {
        m_displayInfo.displayModes[i].width = modes[i]->w;
        m_displayInfo.displayModes[i].height = modes[i]->h;
        std::cout << "[Display] " << m_displayInfo.displayModes[i].width << "x" << m_displayInfo.displayModes[i].height << std::endl;
    }

    m_nativeMode = modes[0];                                                                                // Ustawia tryb natywny

    // Wstępna lista trybów domyślnych
    std::vector<Mode> videoModes = {
        {320, 200, "(16:10)"},
        {320, 240, "(4:3)"},
        {640, 480, "(4:3)"},
        {720, 480, "(3:2)"},
        {800, 600, "(4:3)"},
        {960, 640, "(3:2)"},
        {1024, 768, "(4:3)"},
        {1280, 720, "(16:9)"},    // HD
        {1280, 800, "(16:10)"},
        {1280, 1024, "(5:4)"},
        {1360, 768, "(16:9)"},
        {1366, 768, "(16:9)"},
        {1400, 1050, "(4:3)"},
        {1440, 900, "(16:10)"},
        {1440, 960, "(3:2)"},
        {1600, 900, "(16:9)"},
        {1600, 1200, "(4:3)"},
        {1680, 1050, "(16:10)"},
        {1920, 1080, "(16:9)"},   // FullHD
        {1920, 1200, "(16:10)"},
        {2048, 1152, "(16:9)"},
        {2048, 1536, "(4:3)"},
        {2160, 1440, "(3:2)"},
        {2240, 1400, "(16:10)"},
        {2560, 1080, "(21:9)"},
        {2560, 1440, "(16:9)"},   // 2K
        {2560, 1600, "(16:10)"},
        {2560, 1920, "(4:3)"},
        {2560, 2048, "(5:4)"},
        {2880, 1800, "(16:10)"},
        {3000, 2000, "(3:2)"},
        {3200, 1800, "(16:9)"},   // 3K
        {3240, 2160, "(3:2)"},
        {3440, 1440, "(21:9)"},
        {3840, 1080, "(32:9)"},
        {3840, 1600, "(21:9)"},
        {3840, 2160, "(16:9)"},   // 4K
        {4500, 3000, "(3:2)"},
        {5120, 1440, "(32:9)"},
        {5120, 2160, "(21:9)"},
        {5120, 2880, "(16:9)"},   // 5K
        {6016, 3384, "(16:9)"},
        {6144, 3456, "(16:9)"},   // 6K
        {7680, 2160, "(32:9)"},
        {7680, 4320, "(16:9)"}    // 8K
    };

    // Tworzymy kopię listy trybów monitora
    std::vector<Mode> modeCopy = m_displayInfo.displayModes;

    // Wrzucamy listę trybów domyślnych do listy głównej trybów
    for (const auto& videoMode : videoModes) {

        bool found = false;

        for (const auto& mode : modeCopy) {
            if (videoMode.width == mode.width && videoMode.height == mode.height) {
                found = true;
                break;
            }
        }

        if (!found) {
            if (videoMode.width < modeCopy[0].width && videoMode.height < modeCopy[0].height) {     // pomija większe niż natywna
                m_displayInfo.displayModes.push_back(videoMode);
            }
        }

    }

    // Wyliczamy dla każdego trybu współczynnik ratio
    struct R { float value; std::string name; };

    R known[] = {
        {4.0f/3.0f, "(4:3)"},
        {16.0f/10.0f, "(16:10)"},
        {16.0f/9.0f, "(16:9)"},
        {21.0f/9.0f, "(21:9)"},
        {32.0f/9.0f, "(32:9)"},
        {3.0f/2.0f, "(3:2)"},
        {5.0f/4.0f, "(5:4)"}
    };

    for (auto& mode : m_displayInfo.displayModes) {
        float ratio = static_cast<float>(mode.width) / mode.height;

        float bestDiff = std::numeric_limits<float>::max();
        std::string best = "custom";

        for (auto& k : known) {
            float diff = std::abs(ratio - k.value);
            if (diff < bestDiff) {
                bestDiff = diff;
                best = k.name;
            }
        }

        mode.ratio = best;

    }


    // Sortujemy listę od największej
    std::sort(m_displayInfo.displayModes.begin(),
              m_displayInfo.displayModes.end(),
              [](const Mode& a, const Mode& b) {
                  if (a.width != b.width) return a.width > b.width;
                  return a.height > b.height;
              });

    // Oznaczamy tryb natywnej rozdzielczości
    m_displayInfo.displayModes[0].ratio += " (native)";

    std::cout << "[Monitor] " << m_displayInfo.name << std::endl;
    for (int i = 0; i < m_displayInfo.displayModes.size(); i++) {
        std::cout << m_displayInfo.displayModes[i].width << "x" << m_displayInfo.displayModes[i].height << " " << m_displayInfo.displayModes[i].ratio << std::endl;
    }


    SDL_free(displays);

}

const DisplayInfo& Display::GetPrimaryDisplay() const {
    return m_displayInfo;
}

std::vector<Mode> Display::GetDisplayModes() const {
    return m_displayInfo.displayModes;
}

const SDL_DisplayMode* Display::GetDisplayNativeMode() {
    return m_nativeMode;
}

float Display::GetScaling() {
    const SDL_DisplayMode* mcurr = SDL_GetCurrentDisplayMode(m_displayInfo.id);
    if (!mcurr) { return 1.0f; }
    float scaling = static_cast<float>(m_displayInfo.displayModes[0].width) / mcurr->w;
    std::cout << "Scaling: " << scaling << std::endl;
    return scaling;
}
