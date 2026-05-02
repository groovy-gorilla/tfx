#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanInstance {
public:
    void Create();
    void Destroy();

    VkInstance Get() const;

private:
    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

    std::vector<const char*> GetRequiredExtensions();
    void SetupDebugMessenger(VkInstance instance);

};