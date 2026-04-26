#pragma once
#include <string>

// Struktura z ustawieniami
struct Settings {
    const std::string TITLE = "Indigo Engine";
    uint32_t WIDTH = 800;
    uint32_t HEIGHT = 600;
    bool FULLSCREEN = true;
    bool KEEP_ASPECT_RATIO = true;
    const int MAX_FRAMES_IN_FLIGHT = 2;
    VkSampleCountFlagBits MSAA_SAMPLES = VK_SAMPLE_COUNT_1_BIT;
    VkFilter FILTER = VK_FILTER_NEAREST;
};
extern Settings SETTINGS;

