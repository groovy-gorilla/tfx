#include "VulkanDebug.h"
#include "../../Engine/Core/Error/ErrorDialog.h"

#include <iostream>

void VulkanDebug::Create(VkInstance instance) {

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    createInfo.pfnUserCallback = DebugCallback;

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &m_messenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create debug messenger");
    }

    std::cout << "[Vulkan] Debug messenger created" << std::endl;

}

void VulkanDebug::Destroy(VkInstance instance) {
    if (m_messenger != VK_NULL_HANDLE) {
        DestroyDebugUtilsMessengerEXT(instance, m_messenger, nullptr);
        m_messenger = VK_NULL_HANDLE;

        std::cout << "[Vulkan] Debug messenger destroyed" << std::endl;
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebug::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        std::string msg = "Validation Layer\n\n"
        "ID:" + std::string(pCallbackData->pMessageIdName) + "\n\n"
        "Message:\n" + std::string(pCallbackData->pMessage);
        ShowErrorDialog(msg);
    }

    return VK_FALSE;
}

VkResult VulkanDebug::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {

    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }

    return VK_ERROR_EXTENSION_NOT_PRESENT;

}

void VulkanDebug::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {

    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }

}


