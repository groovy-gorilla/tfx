#pragma once

#include <vulkan/vulkan.h>

class VulkanOffscreenResources {
public:
    void Create(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D extent, VkFormat colorFormat, VkFormat depthFormat, VkRenderPass renderPass);
    void Destroy(VkDevice device);
    VkFramebuffer GetFramebuffer() const;
    VkImageView GetColorImageView() const;
    VkExtent2D GetExtent() const;

    VkImage GetColorImage() const { return m_colorImage; }

private:
    VkExtent2D m_extent{};

    VkImage m_colorImage = VK_NULL_HANDLE;
    VkDeviceMemory m_colorMemory = VK_NULL_HANDLE;
    VkImageView m_colorImageView = VK_NULL_HANDLE;

    VkImage m_depthImage = VK_NULL_HANDLE;
    VkDeviceMemory m_depthMemory = VK_NULL_HANDLE;
    VkImageView m_depthImageView = VK_NULL_HANDLE;

    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;


};