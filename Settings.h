#pragma once

#include <string>
#include <vulkan/vulkan.h>

// Struktura z ustawieniami
struct Settings {
    const std::string TITLE = "Vulkan";
    uint32_t WIDTH = 3440;
    uint32_t HEIGHT = 1440;
    bool FULLSCREEN = true;
    bool KEEP_ASPECT_RATIO = false;
    const int MAX_FRAMES_IN_FLIGHT = 2;
    VkSampleCountFlagBits MSAA_SAMPLES = VK_SAMPLE_COUNT_1_BIT;
    VkFilter FILTER = VK_FILTER_NEAREST;
};
extern Settings SETTINGS;

