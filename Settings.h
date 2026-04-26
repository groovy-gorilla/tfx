#pragma once
#include <string>

// Struktura z ustawieniami
struct Settings {
    const std::string TITLE = "Indigo Engine";
    uint32_t WIDTH = 2560;
    uint32_t HEIGHT = 1080;
    bool FULLSCREEN = false;
    bool KEEP_ASPECT_RATIO = false;
    const int MAX_FRAMES_IN_FLIGHT = 2;
    VkSampleCountFlagBits MSAA_SAMPLES = VK_SAMPLE_COUNT_1_BIT;
    VkFilter FILTER = VK_FILTER_NEAREST;
};
extern Settings SETTINGS;

