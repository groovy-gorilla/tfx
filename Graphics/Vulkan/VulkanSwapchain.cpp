#include "VulkanSwapchain.h"

#include <iostream>
#include <stdexcept>
#include <SDL3/SDL.h>

void VulkanSwapchain::Create(VkPhysicalDevice physicalDevice, VkDevice device,VkSurfaceKHR surface, uint32_t width, uint32_t height, uint32_t graphicsQueueFamily, uint32_t presentQueueFamily) {

    // Capabilities
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

    // Formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());

    // Present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

    VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(formats);
    VkPresentModeKHR presentMode = ChoosePresentMode(presentModes);
    VkExtent2D extent = ChooseExtent(capabilities, width, height);

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = { graphicsQueueFamily, presentQueueFamily };

    if (graphicsQueueFamily != presentQueueFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swapchain");
    }

    // 🔥 images
    uint32_t count;
    vkGetSwapchainImagesKHR(device, m_swapchain, &count, nullptr);
    m_images.resize(count);
    vkGetSwapchainImagesKHR(device, m_swapchain, &count, m_images.data());

    m_imageFormat = surfaceFormat.format;
    m_extent = extent;

    std::cout << "[Vulkan] Swapchain created" << std::endl;

}

VkSurfaceFormatKHR VulkanSwapchain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {

    for (const auto& f : formats) {
        if (f.format == VK_FORMAT_B8G8R8A8_SRGB &&
            f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return f;
            }
    }

    return formats[0];

}

VkPresentModeKHR VulkanSwapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& modes) {

    for (const auto& m : modes) {
        if (m == VK_PRESENT_MODE_MAILBOX_KHR)
            return m;
    }

    return VK_PRESENT_MODE_FIFO_KHR;

}

VkExtent2D VulkanSwapchain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) {

    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }

    VkExtent2D extent;
    extent.width = width;
    extent.height = height;

    extent.width = std::max(capabilities.minImageExtent.width,
                           std::min(capabilities.maxImageExtent.width, extent.width));

    extent.height = std::max(capabilities.minImageExtent.height,
                            std::min(capabilities.maxImageExtent.height, extent.height));

    return extent;

}

void VulkanSwapchain::Destroy(VkDevice device) {

    if (m_swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, m_swapchain, nullptr);
        m_swapchain = VK_NULL_HANDLE;
        std::cout << "[Vulkan] Swapchain destroyed" << std::endl;
    }

}

VkSwapchainKHR VulkanSwapchain::Get() const {
    return m_swapchain;
}

const std::vector<VkImage>& VulkanSwapchain::GetImages() const {
    return m_images;
}

VkFormat VulkanSwapchain::GetImageFormat() const {
    return m_imageFormat;
}

VkExtent2D VulkanSwapchain::GetExtent() const {
    return m_extent;
}

