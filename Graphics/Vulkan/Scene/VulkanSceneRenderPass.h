#pragma once

#include <vulkan/vulkan.h>

class VulkanSceneRenderPass {
public:
    void Create(VkDevice device, VkFormat colorFormat, VkFormat depthFormat, VkSampleCountFlagBits samples);
    void Destroy(VkDevice device);

    VkRenderPass Get() const { return m_renderPass; };

private:
    VkRenderPass m_renderPass = VK_NULL_HANDLE;

};

