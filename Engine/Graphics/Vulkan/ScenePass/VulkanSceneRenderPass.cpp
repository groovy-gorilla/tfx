#include "pch.h"
#include "VulkanSceneRenderPass.h"
#include "Debug/ErrorDialog.h"

void VulkanSceneRenderPass::Create(VkDevice device, VkFormat colorFormat, VkFormat depthFormat, AntiAliasing aaMode, VkSampleCountFlagBits samples) {

    std::vector<VkAttachmentDescription> attachments;

    VkAttachmentReference colorRef{};
    VkAttachmentReference depthRef{};
    VkAttachmentReference resolveRef{};
    VkSubpassDescription subpass{};

    switch (aaMode) {

        case AntiAliasing::None:
        case AntiAliasing::SMAA:
        case AntiAliasing::SSAA:
        case AntiAliasing::SSAA_SMAA:
             CreateNormal(attachments, colorRef, depthRef, colorFormat, depthFormat, subpass);
             break;

        case AntiAliasing::MSAA:
        case AntiAliasing::MSAA_SMAA:
             CreateMSAA(attachments, colorRef, depthRef, resolveRef, colorFormat, depthFormat, samples, subpass);
             break;

    }

    // DEPENDECIES
    VkSubpassDependency dep1{};
    dep1.srcSubpass = VK_SUBPASS_EXTERNAL;
    dep1.dstSubpass = 0;
    dep1.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dep1.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dep1.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dep1.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency dep2{};
    dep2.srcSubpass = 0;
    dep2.dstSubpass = VK_SUBPASS_EXTERNAL;
    dep2.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dep2.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dep2.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dep2.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    VkSubpassDependency dependencies[] = { dep1, dep2 };

    // CREATE
    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    createInfo.pAttachments = attachments.data();
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount = 2;
    createInfo.pDependencies = dependencies;

    VK_CHECK(vkCreateRenderPass(device, &createInfo, nullptr, &m_renderPass));

    std::cout << "[Vulkan] Scene-render pass created" << std::endl;

}

void VulkanSceneRenderPass::Destroy(VkDevice device) {

    if (m_renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(device, m_renderPass, nullptr);
        m_renderPass = VK_NULL_HANDLE;
        std::cout << "[Vulkan] Scene-render pass destroyed" << std::endl;
    }

}

void VulkanSceneRenderPass::Begin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent) {

    VkClearValue clearValues[3]{};

    // COLOR
    clearValues[0].color = {0.05f, 0.05f, 0.08f, 1.0f};

    // DEPTH
    clearValues[1].depthStencil = { 1.0f, 0 };

    // RESOLVE COLOR
    clearValues[2].color = { 0.0f, 0.0f, 0.0f, 1.0f };

    VkRenderPassBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = m_renderPass;
    beginInfo.framebuffer = framebuffer;
    beginInfo.renderArea.extent = extent;
    beginInfo.clearValueCount = 3;
    beginInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

}

void VulkanSceneRenderPass::End(VkCommandBuffer commandBuffer) {

    vkCmdEndRenderPass(commandBuffer);

}

void VulkanSceneRenderPass::CreateNormal(std::vector<VkAttachmentDescription>& attachments, VkAttachmentReference& colorRef, VkAttachmentReference& depthRef, VkFormat colorFormat, VkFormat depthFormat, VkSubpassDescription& subpass) {

    // COLOR
    VkAttachmentDescription color{};
    color.format = colorFormat;
    color.samples = VK_SAMPLE_COUNT_1_BIT;
    color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color.finalLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // DEPTH
    VkAttachmentDescription depth{};
    depth.format = depthFormat;
    depth.samples = VK_SAMPLE_COUNT_1_BIT;
    depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth.finalLayout   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

    attachments.push_back(color); // 0
    attachments.push_back(depth); // 1

    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    depthRef.attachment = 1;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // SUBPASS
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;
    subpass.pDepthStencilAttachment = &depthRef;

}

void VulkanSceneRenderPass::CreateMSAA(std::vector<VkAttachmentDescription>& attachments, VkAttachmentReference& colorRef, VkAttachmentReference& depthRef, VkAttachmentReference& resolveRef, VkFormat colorFormat, VkFormat depthFormat, VkSampleCountFlagBits samples, VkSubpassDescription& subpass) {

    // COLOR MSAA
    VkAttachmentDescription msaa{};
    msaa.format = colorFormat;
    msaa.samples = samples;
    msaa.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    msaa.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    msaa.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    msaa.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    msaa.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    msaa.finalLayout   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // DEPTH MSAA
    VkAttachmentDescription depth{};
    depth.format = depthFormat;
    depth.samples = samples;
    depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth.finalLayout   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // RESOLVE (final image → shader)
    VkAttachmentDescription resolve{};
    resolve.format = colorFormat;
    resolve.samples = VK_SAMPLE_COUNT_1_BIT;
    resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    resolve.finalLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    attachments.push_back(msaa);            // 0
    attachments.push_back(depth);           // 1
    attachments.push_back(resolve);         // 2

    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    depthRef.attachment = 1;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    resolveRef.attachment = 2;
    resolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // SUBPASS
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;
    subpass.pDepthStencilAttachment = &depthRef;
    subpass.pResolveAttachments = &resolveRef;

}

