#include "VulkanInstance.h"

#include <iostream>
#include <ostream>
#include <stdexcept>
#include <SDL3/SDL_vulkan.h>
#include "VulkanValidation.h"
#include "../../Engine/Core/ApplicationDesc.h"
#include "../../Graphics/Vulkan/VulkanDebug.h"
#include "../../Engine/Core/Error/ErrorDialog.h"

void VulkanInstance::Create(ApplicationDesc& desc) {

    if (VulkanValidation::ENABLE && !VulkanValidation::CheckSupport()) {
        throw std::runtime_error("Validation layers are not available!");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = desc.title;
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = "Indigo";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = GetRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (VulkanValidation::ENABLE) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanValidation::Layers.size());
        createInfo.ppEnabledLayerNames = VulkanValidation::Layers.data();
        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_instance));

    std::cout << "[Vulkan] Instance created" << std::endl;

}

void VulkanInstance::Destroy() {

    if (m_instance) {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
        std::cout << "[Vulkan] Instance destroyed" << std::endl;
    }

}

VkInstance VulkanInstance::Get() const {
    return m_instance;
}

std::vector<const char*> VulkanInstance::GetRequiredExtensions() {

    uint32_t count;

    const char* const* ext = SDL_Vulkan_GetInstanceExtensions(&count);
    if (!ext) {
        throw std::runtime_error(SDL_GetError());
    }

    std::vector<const char*> extensions(ext, ext + count);

    if (true) { // validation enable
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;

}

void VulkanInstance::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {

    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = VulkanDebug::DebugCallback;

}

