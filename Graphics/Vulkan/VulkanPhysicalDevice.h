#pragma once

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);


class VulkanPhysicalDevice {
public:
    void Pick(VkInstance instance, VkSurfaceKHR surface);

    VkPhysicalDevice Get() const;

    uint32_t GetGraphicsQueueFamily() const;
    uint32_t GetPresentQueueFamily() const;

    void CreateSupportedSampleCounts();
    const std::vector<VkSampleCountFlagBits>& GetSupportedSampleCounts() const;

private:
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    uint32_t m_graphicsQueueFamily = UINT32_MAX;
    uint32_t m_presentQueueFamily = UINT32_MAX;
    std::vector<VkSampleCountFlagBits> m_msaaSamples;

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

};
