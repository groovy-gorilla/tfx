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
        std::vector<VkAttachmentDescription2>& attachments,
        VkAttachmentReference2& colorRef,
        VkAttachmentReference2& depthRef,
        VkFormat colorFormat,
        VkFormat depthFormat,
        VkSubpassDescription2& subpass);

    void CreateMSAA(
        std::vector<VkAttachmentDescription2>& attachments,
        VkAttachmentReference2& colorRef,
        VkAttachmentReference2& depthRef,
        VkAttachmentReference2& resolveRef,
        VkAttachmentReference2& depthResolveRef,
        VkFormat colorFormat,
        VkFormat depthFormat,
        VkSampleCountFlagBits samples,
        VkSubpassDescription2& subpass,
        VkSubpassDescriptionDepthStencilResolve& depthResolveInfo);

};

