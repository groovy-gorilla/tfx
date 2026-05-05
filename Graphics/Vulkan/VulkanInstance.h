#pragma once

#include "../../Engine/Core/ApplicationDesc.h"
#include <vulkan/vulkan.h>
#include <vector>

class VulkanInstance {
public:

    void Create(ApplicationDesc& desc);
    void Destroy();

    VkInstance Get() const;

private:
    VkInstance m_instance = VK_NULL_HANDLE;

    std::vector<const char*> GetRequiredExtensions();
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

};