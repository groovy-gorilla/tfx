#include "VulkanInstance.h"

#include <iostream>
#include <ostream>
#include <stdexcept>
#include <SDL3/SDL_vulkan.h>
#include "../../Graphics/Vulkan/VulkanDebug.h"
#include "../../Engine/Core/Error/ErrorDialog.h"

std::vector<const char*> VulkanInstance::GetRequiredExtensions() {

    uint32_t count;

    const char* const* ext = SDL_Vulkan_GetInstanceExtensions(&count);
    if (!ext) {
        throw std::runtime_error(SDL_GetError());
    }

    std::vector<const char*> extensions(ext, ext + count);

    #ifndef NDEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    #endif

    return extensions;

}

void VulkanInstance::Create() {

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Indigo Engine";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = "Indigo";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = GetRequiredExtensions();

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = 0;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (ENABLE_VALIDATION) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_instance));

    SetupDebugMessenger();

    std::cout << "[Vulkan] Instance created" << std::endl;

}

void VulkanInstance::Destroy() {

    if (ENABLE_VALIDATION) {
        DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }

    if (m_instance) {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
        std::cout << "[Vulkan] Instance destroyed" << std::endl;
    }

}

VkInstance VulkanInstance::Get() const {
    return m_instance;
}

void VulkanInstance::SetupDebugMessenger() {
    if (!ENABLE_VALIDATION) return;

    VkDebugUtilsMessengerCreateInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    info.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    info.pfnUserCallback = DebugCallback;

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");

    if (func) {
        func(m_instance, &info, nullptr, &m_debugMessenger);
    }
}
