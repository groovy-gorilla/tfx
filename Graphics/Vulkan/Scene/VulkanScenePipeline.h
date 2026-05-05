#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class VulkanScenePipeline {
public:
    void Create(VkDevice device, VkExtent2D extent, VkRenderPass renderPass, VkSampleCountFlagBits samples);
    void Destroy(VkDevice device);

    VkPipeline Get() const { return m_pipeline; }
    VkPipelineLayout GetLayout() const {return m_layout;}

private:
    VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);

    VkPipelineLayout m_layout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;

};
