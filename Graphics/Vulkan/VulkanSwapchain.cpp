#include "VulkanSwapchain.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <SDL3/SDL.h>
#include "VulkanPhysicalDevice.h"
#include "../../Engine/Core/Error/ErrorDialog.h"
#include "../../Engine/Core/Window.h"

void VulkanSwapchain::Create(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface,  VkExtent2D extent, uint32_t graphicsQueueFamily, uint32_t presentQueueFamily) {

    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D ext = ChooseSwapExtent(swapChainSupport.capabilities, extent);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = ext;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = { graphicsQueueFamily, presentQueueFamily };

    if (graphicsQueueFamily != presentQueueFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_swapchain));

    // Images
    uint32_t count;
    VK_CHECK(vkGetSwapchainImagesKHR(device, m_swapchain, &count, nullptr));
    m_images.resize(count);
    VK_CHECK(vkGetSwapchainImagesKHR(device, m_swapchain, &count, m_images.data()));

    m_imageFormat = surfaceFormat.format;
    m_extent = ext;

    std::cout << "[Vulkan] Swapchain created" << std::endl;

    CreateImageViews(device);

}

VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {

    // 1. Najlepszy wybór (sRGB)
    for (const auto& f : formats) {
        if (f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return f;
        }
    }

    // 2. Drugi wybór (UNORM + sRGB colorspace)
    for (const auto& f : formats) {
        if (f.format == VK_FORMAT_B8G8R8A8_UNORM && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return f;
        }
    }

    // 3. Ostateczność
    return formats[0];

}

VkPresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& modes) {

    for (const auto& m : modes) {
        if (m == VK_PRESENT_MODE_MAILBOX_KHR)
            return m;
    }

    return VK_PRESENT_MODE_FIFO_KHR;

}

VkExtent2D VulkanSwapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,  VkExtent2D extent) {

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return extent;

}

void VulkanSwapchain::Destroy(VkDevice device) {

    DestroyImageViews(device);

    if (m_swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, m_swapchain, nullptr);
        m_swapchain = VK_NULL_HANDLE;
        std::cout << "[Vulkan] Swapchain destroyed" << std::endl;
    }

}

void VulkanSwapchain::CreateImageViews(VkDevice device) {

    m_imageViews.resize(m_images.size());

    for (size_t i = 0; i < m_images.size(); i++) {

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

        viewInfo.image = m_images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = m_imageFormat;

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

void VulkanSwapchain::DestroyImageViews(VkDevice device) {

    for (auto view : m_imageViews) {
        vkDestroyImageView(device, view, nullptr);
    }

    m_imageViews.clear();

    std::cout << "[Vulkan] Image views destroyed" << std::endl;

}



