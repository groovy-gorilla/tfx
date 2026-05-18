#pragma once

#include <vulkan/vulkan.h>

#include "Graphics/Vulkan/Resources//VulkanTextureDescriptor.h"
#include "Graphics/Vulkan/Resources/RenderTarget.h"

class ApplicationDesc;

class VulkanSMAARenderPass {
public:

    void Create(VkPhysicalDevice physicalDevice,
                VkDevice device,
                VkExtent2D extent,
                RenderTarget& outputColor,
                VkFormat blendFormat,
                ApplicationDesc& desc,
                VkCommandPool commandPool,
                VkQueue graphicsQueue);

    void Render(VkCommandBuffer commandBuffer,
                VkExtent2D extent,
                uint32_t currentFrame,
                RenderTarget& inputColor);

    void Destroy(VkDevice device);

private:

    // INTERNAL RENDERING
    void RenderEdgePass(VkCommandBuffer commandBuffer,
                        VkExtent2D extent,
                        uint32_t currentFrame);

    void RenderBlendPass(VkCommandBuffer commandBuffer,
                         VkExtent2D extent,
                         uint32_t currentFrame);

    void RenderNeighborhoodPass(VkCommandBuffer commandBuffer,
                                VkExtent2D extent,
                                uint32_t currentFrame);

    // EDGE PASS
    void CreateEdgeRenderPass(VkDevice device);

    void CreateEdgeFramebuffer(VkDevice device,
                               VkExtent2D extent);

    void CreateEdgeDescriptors(VkDevice device,
                               ApplicationDesc& desc);

    void CreateEdgePipeline(VkDevice device,
                            VkExtent2D extent);

    // BLEND PASS
    void CreateBlendRenderPass(VkDevice device);

    void CreateBlendFramebuffer(VkDevice device,
                                VkExtent2D extent);

    void CreateBlendDescriptors(VkDevice device,
                                 ApplicationDesc& desc);

    void CreateBlendPipeline(VkDevice device,
                             VkExtent2D extent);

    // NEIGHBORHOOD PASS
    void CreateNeighborhoodRenderPass(VkDevice device, RenderTarget& outputColor);

    void CreateNeighborhoodFramebuffer(VkDevice device,
                                       VkExtent2D extent,
                                       RenderTarget& outputColor);

    void CreateNeighborhoodDescriptors(VkDevice device,
                                       ApplicationDesc& desc);

    void CreateNeighborhoodPipeline(VkDevice device,
                                    VkExtent2D extent);

private:

    // DEVICE
    VkDevice m_device = VK_NULL_HANDLE;

    // INTERMEDIATE TARGETS
    RenderTarget m_edgeColor{};
    RenderTarget m_blendColor{};

    // SMAA LOOKUP TEXTURES
    RenderTarget m_areaTexture{};
    RenderTarget m_searchTexture{};

    // EDGE PASS
    VkRenderPass m_edgeRenderPass = VK_NULL_HANDLE;
    VkFramebuffer m_edgeFramebuffer = VK_NULL_HANDLE;

    VkPipelineLayout m_edgePipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_edgePipeline = VK_NULL_HANDLE;

    VulkanTextureDescriptor m_edgeDescriptor{};
    VkDescriptorSetLayout m_edgeDescriptorLayout = VK_NULL_HANDLE;

    // BLEND PASS
    VkRenderPass m_blendRenderPass = VK_NULL_HANDLE;
    VkFramebuffer m_blendFramebuffer = VK_NULL_HANDLE;

    VkPipelineLayout m_blendPipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_blendPipeline = VK_NULL_HANDLE;

    VulkanTextureDescriptor m_blendDescriptor{};
    VkDescriptorSetLayout m_blendDescriptorLayout = VK_NULL_HANDLE;

    // NEIGHBORHOOD PASS
    VkRenderPass m_neighborhoodRenderPass = VK_NULL_HANDLE;
    VkFramebuffer m_neighborhoodFramebuffer = VK_NULL_HANDLE;

    VkPipelineLayout m_neighborhoodPipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_neighborhoodPipeline = VK_NULL_HANDLE;

    VulkanTextureDescriptor m_neighborhoodDescriptor{};
    VkDescriptorSetLayout m_neighborhoodDescriptorLayout = VK_NULL_HANDLE;

    VkCommandPool m_commandPool{};
    VkQueue m_graphicsQueue{};

};