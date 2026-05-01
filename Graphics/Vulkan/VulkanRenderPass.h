#pragma once

#include <vulkan/vulkan.h>

class VulkanRenderPass {
public:
    void Create(VkDevice device, VkFormat swapchainFormat);
    void Destroy(VkDevice device);

    VkRenderPass Get() const;

private:
    VkRenderPass m_renderPass = VK_NULL_HANDLE;

};

