#include "VulkanPhysicalDevice.h"

#include <iostream>
#include <ostream>
#include <stdexcept>
#include <vector>

void VulkanPhysicalDevice::Pick(VkInstance instance, VkSurfaceKHR surface) {

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("No Vulkan-compatible GPU found.");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (IsDeviceSuitable(device, surface)) {
            m_physicalDevice = device;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("No suitable GPU found.");
    }

    std::cout << "[Vulkan] Physical device selected" << std::endl;

}

bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {

    FindQueueFamilies(device, surface);

    return m_graphicsQueueFamily != UINT32_MAX && m_presentQueueFamily != UINT32_MAX;

}

void VulkanPhysicalDevice::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {

    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);

    std::vector<VkQueueFamilyProperties> queues(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queues.data());

    for (uint32_t i = 0; i < queueCount; i++) {

        // Graphics queue
        if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            m_graphicsQueueFamily = i;
        }

        // Present queue (czy może wyświetlać na ekran)
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) {
            m_presentQueueFamily = i;
        }

        // Jeśli mamy oba - kończymy
        if (m_graphicsQueueFamily != UINT32_MAX && m_presentQueueFamily != UINT32_MAX) {
            break;
        }

    }

}

VkPhysicalDevice VulkanPhysicalDevice::Get() const {
    return m_physicalDevice;
}

uint32_t VulkanPhysicalDevice::GetGraphicsQueueFamily() const {
    return m_graphicsQueueFamily;
}

uint32_t VulkanPhysicalDevice::GetPresentQueueFamily() const {
    return m_presentQueueFamily;
}
