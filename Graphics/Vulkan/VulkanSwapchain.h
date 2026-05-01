#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <SDL3/SDL.h>

class VulkanSwapchain {
public:
    void Create(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, uint32_t width, uint32_t height, uint32_t graphicsQueueFamily, uint32_t presentQueueFamily);
    void Destroy(VkDevice device);

    VkSwapchainKHR Get() const;
    const std::vector<VkImage>& GetImages() const;
    VkFormat GetImageFormat() const;
    VkExtent2D GetExtent() const;

private:
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_images;
    VkFormat m_imageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D m_extent = {};

    VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
    VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& modes);
    VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

};
