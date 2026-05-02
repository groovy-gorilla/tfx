#include "VulkanInstance.h"

#include <iostream>
#include <ostream>
#include <stdexcept>
#include <SDL3/SDL_vulkan.h>
#include "../../Graphics/Vulkan/VulkanDebug.h"

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

    auto extensions = GetRequiredExtensions();

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    #ifndef NDEBUG
    const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
    createInfo.enabledLayerCount = 1;
    createInfo.ppEnabledLayerNames = layers;
    #else
    createInfo.enabledLayerCount = 0;
    #endif

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if (ENABLE_VALIDATION) {
        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
        throw std::runtime_error(SDL_GetError());
    }


    SetupDebugMessenger(m_instance);

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

void VulkanInstance::SetupDebugMessenger(VkInstance instance) {
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
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (func) {
        func(instance, &info, nullptr, &m_debugMessenger);
    }
}
