#pragma once

#include <vector>

namespace VulkanValidation {

#ifdef NDEBUG
    inline constexpr bool ENABLE = false;
#else
    inline constexpr bool ENABLE = true;
#endif

    // lista warstw
    extern const std::vector<const char*> Layers;

    // sprawdzenie dostępności
    bool CheckSupport();

}

