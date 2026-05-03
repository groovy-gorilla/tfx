#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

class VulkanPipeline {
public:
    void Create(VkDevice device, VkExtent2D extent, VkRenderPass renderPass);
    void Destroy(VkDevice device);

    VkPipeline Get() const { return m_pipeline; }

private:
    VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);
    std::vector<char> ReadFile(const std::string& filename);

    VkPipelineLayout m_layout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
};
