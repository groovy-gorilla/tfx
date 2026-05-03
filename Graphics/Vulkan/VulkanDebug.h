#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#ifdef NDEBUG
const bool ENABLE_VALIDATION = false;
#else
const bool ENABLE_VALIDATION = true;
#endif


const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

// Callback
VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* data,
    void* userData
);

// Tworzenie debug messengera (wrapper na extension)
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger
);


// Niszczenie debug messengera
void DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator
);

bool checkValidationLayerSupport();

// Helper do wypełnienia create info
void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

