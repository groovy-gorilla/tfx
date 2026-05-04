#include "VulkanOffscreenFramebuffers.h"

#include <iostream>
#include <ostream>
#include <stdexcept>

void VulkanOffscreenFramebuffers::Create(VkDevice device, VkRenderPass renderPass, const std::vector<VkImageView>& imageViews, VkExtent2D extent) {

    m_framebuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); i++) {

        VkImageView attachments[] = {
            imageViews[i]
        };

        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = renderPass;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = attachments;
        createInfo.width  = extent.width;
        createInfo.height = extent.height;
        createInfo.layers = 1;

        if (vkCreateFramebuffer(device, &createInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer");
        }
    }

    std::cout << "[Vulkan] Framebuffers created" << std::endl;

}

void VulkanOffscreenFramebuffers::Destroy(VkDevice device) {

    for (auto fb : m_framebuffers) {
        vkDestroyFramebuffer(device, fb, nullptr);
    }

    m_framebuffers.clear();

    std::cout << "[Vulkan] Framebuffers destroyed" << std::endl;

}

const std::vector<VkFramebuffer>& VulkanOffscreenFramebuffers::Get() const {
    return m_framebuffers;
}

