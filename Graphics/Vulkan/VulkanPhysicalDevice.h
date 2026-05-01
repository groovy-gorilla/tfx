#pragma once

#include <vulkan/vulkan.h>

class VulkanPhysicalDevice {
public:
    void Pick(VkInstance instance, VkSurfaceKHR surface);

    VkPhysicalDevice Get() const;

    uint32_t GetGraphicsQueueFamily() const;
    uint32_t GetPresentQueueFamily() const;

private:
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    uint32_t m_graphicsQueueFamily = UINT32_MAX;
    uint32_t m_presentQueueFamily = UINT32_MAX;

    bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
    void FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

};
