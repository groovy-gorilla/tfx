#pragma once

#include "Core/ApplicationDesc.h"

class VulkanSceneRenderPass {
public:
    void Create(
        VkDevice device,
        VkFormat colorFormat,
        VkFormat depthFormat,
        AntiAliasing aaMode,
        VkSampleCountFlagBits samples);

    void Destroy(VkDevice device);

    void Begin(
        VkCommandBuffer commandBuffer,
        VkFramebuffer framebuffer,
        VkExtent2D extent);

    void End(
        VkCommandBuffer commandBuffer);

    [[nodiscard]] VkRenderPass Get() const { return m_renderPass; }


private:
    VkRenderPass m_renderPass = VK_NULL_HANDLE;

    void CreateNormal(
        std::vector<VkAttachmentDescription>& attachments,
        VkAttachmentReference& colorRef,
        VkAttachmentReference& depthRef,
        VkFormat colorFormat,
        VkFormat depthFormat,
        VkSubpassDescription& subpass);

    void CreateMSAA(
        std::vector<VkAttachmentDescription>& attachments,
        VkAttachmentReference& colorRef,
        VkAttachmentReference& depthRef,
        VkAttachmentReference& resolveRef,
        VkFormat colorFormat,
        VkFormat depthFormat,
        VkSampleCountFlagBits samples,
        VkSubpassDescription& subpass);

};

