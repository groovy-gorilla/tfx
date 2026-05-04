#pragma once

#include <vulkan/vulkan.h>

struct ApplicationDesc {

    const char* title                    = "Indigo Engine";

    uint32_t width                      = 800;
    uint32_t height                     = 600;

    float scaling                       = 1.0f;

    bool fullscreen                     = true;
    bool aspectRatio                    = false;
    bool vsync                          = true;

    const uint32_t maxFramesInFlight    = 2;
    VkSampleCountFlagBits msaaSamples   = VK_SAMPLE_COUNT_1_BIT;
    VkFilter filter                     = VK_FILTER_NEAREST;

};