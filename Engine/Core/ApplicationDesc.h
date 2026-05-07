#pragma once

#include <vulkan/vulkan.h>

enum class AntiAliasing {
    None,
    MSAA,
    SSAA,
    MSAA_TAA,
    SSAA_TAA
};

enum class TextureFilter {
    Nearest,
    Linear
};

struct ApplicationDesc {

    const char* TITLE                   = "Indigo Engine";

    uint32_t WIDTH                      = 320;
    uint32_t HEIGHT                     = 200;

    bool FULLSCREEN                     = true;
    bool ASPECT_RATIO                   = false;
    bool VSYNC                          = true;
    TextureFilter FILTER                = TextureFilter::Linear;
    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    // AntiAliasing
    AntiAliasing AA_MODE                = AntiAliasing::None;

    // MSAA
    VkSampleCountFlagBits MSAA_SAMPLES  = VK_SAMPLE_COUNT_1_BIT;

    // SSAA
    float SSAA_SCALE                    = 1.0f;     // scale choice: 1.0 - 1.25 - 1.5 - 2.0

};