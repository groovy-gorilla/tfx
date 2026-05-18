#pragma once

#include "Graphics/Vulkan/Resources/VulkanTextureDescriptor.h"

struct ApplicationDesc;

struct PostPushConstants {
    int hdrEnable;
    float exposure;
    int dithering;
};

class VulkanPostRenderPass {
public:
    void Create(
        VkDevice device,
        VkExtent2D extent,
        VkFormat swapchainFormat,
        ApplicationDesc desc);

    void Destroy(VkDevice device);

    void Render(
        VkDevice device,
        uint32_t frameIndex,
        VkCommandBuffer commandBuffer,
        RenderTarget& inputColor,
        VkFramebuffer framebuffer,
        VkExtent2D extent,
        ApplicationDesc& desc,
        float exposure);

    [[nodiscard]] VkRenderPass Get() const { return m_renderPass; }
    [[nodiscard]] VulkanTextureDescriptor GetDescriptor() const { return m_sceneDescriptor; }

private:
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    VulkanTextureDescriptor m_sceneDescriptor;

};


