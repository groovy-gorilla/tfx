#pragma once

#include "Core/ApplicationDesc.h"

class VulkanScenePipeline {
public:
    void Create(VkDevice device, VkRenderPass renderPass, AntiAliasing aaMode, VkSampleCountFlagBits samples);
    void Destroy(VkDevice device);

    void Bind(VkCommandBuffer commandBuffer);

    [[nodiscard]] VkPipeline Get() const { return m_pipeline; }
    [[nodiscard]] VkPipelineLayout GetLayout() const {return m_layout;}

private:
    VkPipelineLayout m_layout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;

};
