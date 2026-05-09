#pragma once

#include "pch.h"

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

    const char* TITLE                   = "TFX";

    uint32_t WIDTH                      = 800;
    uint32_t HEIGHT                     = 600;

    bool FULLSCREEN                     = false;
    bool ASPECT_RATIO                   = true;
    bool VSYNC                          = true;
    TextureFilter FILTER                = TextureFilter::Nearest;
    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    // AntiAliasing
    AntiAliasing AA_MODE                = AntiAliasing::None;

    // MSAA
    VkSampleCountFlagBits MSAA_SAMPLES  = VK_SAMPLE_COUNT_1_BIT;

    // SSAA
    float SSAA_SCALE                    = 1.0f;     // scale choice: 1.0 - 1.25 - 1.5 - 2.0
                                                    // UWAGA! Wartości ustawiamy tylko przy AntiAliasing::SSAA lub SSAA_TAA
                                                    // W pozostałych przypadkach musi być = 1.0f

    // FLAGI
    bool TAKE_SCREENSHOT                = false;

};

// *****************************************************************
//                            UWAGA !!!
// *****************************************************************
//
//      Dla AA::None:
//          MSAA_SAMPLES = VK_SAMPLE_COUNT_1_BIT
//          SSAA_SCALE = 1.0f
//
//      Dla AA::MSAA
//          MSAA_SAMPLES = VK_SAMPLE_COUNT_16_BIT
//          SSAA_SCALE = 1.0f
//
//      Dla AA::SSAA
//          MSAA_SAMPLES = VK_SAMPLE_COUNT_1_BIT
//          SSAA_SCALE = 2.0f
//
// *****************************************************************
