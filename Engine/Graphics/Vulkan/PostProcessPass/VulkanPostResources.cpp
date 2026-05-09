#include "pch.h"
#include "VulkanPostResources.h"

void VulkanPostResources::Create(VkDevice device, VkRenderPass renderPass, VkExtent2D extent, const std::vector<VkImageView>& imageViews) {

    for (auto view : imageViews) {

        VkFramebuffer framebuffer;

        VkFramebufferCreateInfo fb{};
        fb.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fb.renderPass = renderPass;
        fb.attachmentCount = 1;
        fb.pAttachments = &view;
        fb.width = extent.width;
        fb.height = extent.height;
        fb.layers = 1;

        vkCreateFramebuffer(device, &fb, nullptr, &framebuffer);

        m_framebuffers.push_back(framebuffer);

    }

}

void VulkanPostResources::Destroy(VkDevice device) {

    for (auto framebuffer : m_framebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    m_framebuffers.clear();

}

