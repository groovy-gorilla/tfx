#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "../../Engine/Core/Window.h"

class VulkanSwapchain {
public:
    void Create(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface,  VkExtent2D extent, uint32_t graphicsQueueFamily, uint32_t presentQueueFamily);
    void Destroy(VkDevice device);

    VkSwapchainKHR Get() const { return m_swapchain; };
    const std::vector<VkImage>& GetImages() const { return m_images; };
    const std::vector<VkImageView>& GetImageViews() const { return m_imageViews; }
    VkFormat GetImageFormat() const { return m_imageFormat; };
    VkExtent2D GetExtent() const { return m_extent; };

private:
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    VkFormat m_imageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D m_extent = {};


    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& modes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,  VkExtent2D extent);
    void CreateImageViews(VkDevice device);
    void DestroyImageViews(VkDevice device);

};
