#pragma once

#include <vulkan/vulkan.h>

class VulkanCommandPool {
public:
    void Create(VkDevice device, uint32_t graphicsQueueFamily);
    void Destroy(VkDevice device);

    VkCommandPool Get() const;

private:
    VkCommandPool m_commandPool = VK_NULL_HANDLE;

};