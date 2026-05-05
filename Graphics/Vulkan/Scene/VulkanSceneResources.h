#pragma once

#include <vulkan/vulkan.h>
#include "../../../Engine/Core/Window.h"

class VulkanSceneResources {
public:
    void Create(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent, VkFormat colorFormat, VkFormat depthFormat, VkSampleCountFlagBits samples, VkRenderPass renderPass);
    void Destroy(VkDevice device);

    VkFramebuffer GetFramebuffer() const { return m_framebuffer; };
    VkImageView GetColorImageView() const { return (m_samples == VK_SAMPLE_COUNT_1_BIT) ? m_colorImageView : m_resolveImageView; };
    VkImage GetColorImage() const { return (m_samples == VK_SAMPLE_COUNT_1_BIT) ? m_colorImage : m_resolveImage; };
    VkExtent2D GetExtent() const { return m_extent; };



private:
    VkExtent2D m_extent{};
    VkSampleCountFlagBits m_samples = VK_SAMPLE_COUNT_1_BIT;

    // NO MSAA - color
    VkImage m_colorImage = VK_NULL_HANDLE;
    VkDeviceMemory m_colorMemory = VK_NULL_HANDLE;
    VkImageView m_colorImageView = VK_NULL_HANDLE;

    // MSAA - color buffer
    VkImage m_msaaImage = VK_NULL_HANDLE;
    VkDeviceMemory m_msaaMemory = VK_NULL_HANDLE;
    VkImageView m_msaaImageView = VK_NULL_HANDLE;

    // MSAA → resolve (final texture)
    VkImage m_resolveImage = VK_NULL_HANDLE;
    VkDeviceMemory m_resolveMemory = VK_NULL_HANDLE;
    VkImageView m_resolveImageView = VK_NULL_HANDLE;

    // DEPTH
    VkImage m_depthImage = VK_NULL_HANDLE;
    VkDeviceMemory m_depthMemory = VK_NULL_HANDLE;
    VkImageView m_depthImageView = VK_NULL_HANDLE;

    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

    void CreateImage(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits samples, VkImage& image, VkDeviceMemory& memory);
    VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect);


};