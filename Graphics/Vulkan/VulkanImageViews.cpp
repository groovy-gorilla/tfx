#include "VulkanImageViews.h"

#include <iostream>
#include <ostream>
#include <stdexcept>

void VulkanImageViews::Create(VkDevice device, const std::vector<VkImage>& images, VkFormat format) {

    m_imageViews.resize(images.size());

    for (size_t i = 0; i < images.size(); i++) {

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

        viewInfo.image = images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;

        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &viewInfo, nullptr, &m_imageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image view");
        }

    }

    std::cout << "[Vulkan] Image views created" << std::endl;

}

void VulkanImageViews::Destroy(VkDevice device) {

    for (auto view : m_imageViews) {
        vkDestroyImageView(device, view, nullptr);
    }

    m_imageViews.clear();

    std::cout << "[Vulkan] Image views destroyed" << std::endl;

}

const std::vector<VkImageView>& VulkanImageViews::Get() const {
    return m_imageViews;
}
