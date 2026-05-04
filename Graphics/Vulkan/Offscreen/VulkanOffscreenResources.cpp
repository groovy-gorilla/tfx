#include "VulkanOffscreenResources.h"

#include <array>
#include <iostream>
#include <ostream>

#include "../../../Engine/Core/Error/ErrorDialog.h"
#include "../VulkanUtils.h"


void VulkanOffscreenResources::Create(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D extent, VkFormat colorFormat, VkFormat depthFormat, VkRenderPass renderPass) {

    m_extent = extent;

    // =========================
    // COLOR IMAGE
    // =========================
    VkImageCreateInfo colorImageInfo{};
    colorImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    colorImageInfo.imageType = VK_IMAGE_TYPE_2D;
    colorImageInfo.extent.width = extent.width;
    colorImageInfo.extent.height = extent.height;
    colorImageInfo.extent.depth = 1;
    colorImageInfo.mipLevels = 1;
    colorImageInfo.arrayLayers = 1;
    colorImageInfo.format = colorFormat;
    colorImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    colorImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorImageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    colorImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    VK_CHECK(vkCreateImage(device, &colorImageInfo, nullptr, &m_colorImage));

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(device, m_colorImage, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = FindMemoryType(
        physicalDevice,
        memReq.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &m_colorMemory));
    VK_CHECK(vkBindImageMemory(device, m_colorImage, m_colorMemory, 0));

    // IMAGE VIEW
    VkImageViewCreateInfo colorViewInfo{};
    colorViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    colorViewInfo.image = m_colorImage;
    colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    colorViewInfo.format = colorFormat;
    colorViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    colorViewInfo.subresourceRange.levelCount = 1;
    colorViewInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(device, &colorViewInfo, nullptr, &m_colorImageView));

    // =========================
    // DEPTH IMAGE
    // =========================
    VkImageCreateInfo depthImageInfo = colorImageInfo;
    depthImageInfo.format = depthFormat;
    depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depthImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VK_CHECK(vkCreateImage(device, &depthImageInfo, nullptr, &m_depthImage));

    vkGetImageMemoryRequirements(device, m_depthImage, &memReq);

    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = FindMemoryType(
        physicalDevice,
        memReq.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &m_depthMemory));
    VK_CHECK(vkBindImageMemory(device, m_depthImage, m_depthMemory, 0));

    VkImageViewCreateInfo depthViewInfo{};
    depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    depthViewInfo.image = m_depthImage;
    depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depthViewInfo.format = depthFormat;
    depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    depthViewInfo.subresourceRange.levelCount = 1;
    depthViewInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(device, &depthViewInfo, nullptr, &m_depthImageView));

    // =========================
    // FRAMEBUFFER
    // =========================
    std::array<VkImageView, 2> attachments = {
        m_colorImageView,
        m_depthImageView
    };

    VkFramebufferCreateInfo fbInfo{};
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.renderPass = renderPass;
    fbInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    fbInfo.pAttachments = attachments.data();
    fbInfo.width = extent.width;
    fbInfo.height = extent.height;
    fbInfo.layers = 1;

    VK_CHECK(vkCreateFramebuffer(device, &fbInfo, nullptr, &m_framebuffer));

    std::cout << "[Vulkan] Offscreen resources created" << std::endl;

}

void VulkanOffscreenResources::Destroy(VkDevice device) {

    if (m_framebuffer != VK_NULL_HANDLE) vkDestroyFramebuffer(device, m_framebuffer, nullptr);

    if (m_colorImageView != VK_NULL_HANDLE) vkDestroyImageView(device, m_colorImageView, nullptr);
    if (m_depthImageView != VK_NULL_HANDLE) vkDestroyImageView(device, m_depthImageView, nullptr);

    if (m_colorImage != VK_NULL_HANDLE) vkDestroyImage(device, m_colorImage, nullptr);
    if (m_depthImage != VK_NULL_HANDLE) vkDestroyImage(device, m_depthImage, nullptr);

    if (m_colorMemory != VK_NULL_HANDLE) vkFreeMemory(device, m_colorMemory, nullptr);
    if (m_depthMemory != VK_NULL_HANDLE) vkFreeMemory(device, m_depthMemory, nullptr);

    std::cout << "[Vulkan] Offscreen resources destroyed" << std::endl;

}

VkFramebuffer VulkanOffscreenResources::GetFramebuffer() const {
    return m_framebuffer;
}

VkImageView VulkanOffscreenResources::GetColorImageView() const {
    return m_colorImageView;
}

VkExtent2D VulkanOffscreenResources::GetExtent() const {
    return m_extent;
}




