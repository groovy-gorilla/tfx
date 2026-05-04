#pragma once

#include <vulkan/vulkan.h>

class VulkanOffscreenRenderPass {
public:
    void Create(VkDevice device, VkFormat colorFormat, VkFormat depthFormat);
    void Destroy(VkDevice device);

    VkRenderPass Get() const;

private:
    VkRenderPass m_renderPass = VK_NULL_HANDLE;

};

