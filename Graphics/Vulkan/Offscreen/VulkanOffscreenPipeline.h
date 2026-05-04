#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class VulkanOffscreenPipeline {
public:
    void Create(VkDevice device, VkExtent2D extent, VkRenderPass offscreenRenderPass);
    void Destroy(VkDevice device);

    VkPipeline Get() const { return m_pipeline; }

private:
    VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);

    VkPipelineLayout m_layout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
};
