#pragma once

#include <vulkan/vulkan.h>
#include "../Resources/VulkanTextureDescriptor.h"
#include "../../../Engine/Core/ApplicationDesc.h"

class VulkanPostRenderPass {
public:
    void Create(
        VkDevice device,
        VkExtent2D extent,
        VkFormat swapchainFormat,
        RenderTarget& sceneColor,
        RenderTarget& sceneDepth,
        ApplicationDesc& desc);

    void Destroy(VkDevice device);

    void Render(
        VkCommandBuffer commandBuffer,
        VkFramebuffer framebuffer,
        VkExtent2D extent,
        ApplicationDesc& desc);

    VkRenderPass Get() const { return m_renderPass; }
    VulkanTextureDescriptor GetDescriptor() const { return m_sceneDescriptor; }

private:
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    VulkanTextureDescriptor m_sceneDescriptor;

};

