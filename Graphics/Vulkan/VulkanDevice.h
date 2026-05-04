#pragma once

#include <vulkan/vulkan.h>
#include "VulkanPhysicalDevice.h"

class VulkanDevice {
public:
    void Create(VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamily, uint32_t presentQueueFamily);
    void Destroy();

    VkDevice Get() const;

    VkQueue GetGraphicsQueue() const;
    VkQueue GetPresentQueue() const;

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;

};