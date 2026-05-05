#include "VulkanPhysicalDevice.h"

#include <algorithm>
#include <iostream>
#include <ostream>
#include <set>
#include <stdexcept>
#include <vector>

void VulkanPhysicalDevice::Pick(VkInstance instance, VkSurfaceKHR surface) {


    // Wylicza karty graficzne wspierające Vulkan
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    // Jeśli nie ma to błąd
    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    // Pobiera listę kart graficznych wspierających Vulkan
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // Tworzy drugą listę kart graficznych z flagą używalności
    struct GPUs {
        VkPhysicalDevice physicalDevice;
        bool usable = true;
    };
    std::vector<GPUs> GPU_List;
    GPU_List.resize(deviceCount);

    // Przerzuca główną listę do drugiej
    for (int i = 0; i < deviceCount; i++) {
        GPU_List[i].physicalDevice = devices[i];
    }

    // Sprawdza każdą kartę pod względem używalności
    for (auto& gpu : GPU_List) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(gpu.physicalDevice, &deviceProperties);

        // Jeśli to software CPU - odpada
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) {
            gpu.usable = false;
        }

        // Jeśli nie obsługuje kolejek graphicsQueue i presetnQueue - odpada
        QueueFamilyIndices indices = FindQueueFamilies(gpu.physicalDevice, surface);
        if (!indices.graphicsFamily.has_value() || !indices.presentFamily.has_value()) {
            gpu.usable = false;
        }

        // Jeśli nie obsługuje wymaganych rozszerzeń - odpada
        if (!CheckDeviceExtensionSupport(gpu.physicalDevice)) {
            gpu.usable = false;
        }

        // Jeśli nie obsługuje swapchain - odpada
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(gpu.physicalDevice, surface);
        if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) {
            gpu.usable = false;
        }

    }

    // Sprawdza właściwości każdego używalnego urządzenia
    VkPhysicalDevice integrated = VK_NULL_HANDLE;
    for (auto& gpu : GPU_List) {

        if (gpu.usable) {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(gpu.physicalDevice, &deviceProperties);

            // Jeśli jest dedykowana to wybiera pierwszą z listy
            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                m_physicalDevice = gpu.physicalDevice;
                break;
            }

            // lub zintegrowaną jak nie ma discrete gpu
            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
                integrated = gpu.physicalDevice;
            }
        }

    }

    // Jeśli jest zinegrowana a nie ma dedykowanej to wybiera zintegrowaną
    if (m_physicalDevice == VK_NULL_HANDLE && integrated != VK_NULL_HANDLE) {
        m_physicalDevice = integrated;
    }

    // Jeśli nie ma w ogóle to znaczy, że nie ma GPU spełniającego wymagania
    if (m_physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("No suitable GPU found.");
    }

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(m_physicalDevice, &physicalDeviceProperties);
    std::cout << "[Vulkan] Physical device selected: " << physicalDeviceProperties.deviceName << std::endl;

}


QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {

    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {

        // graphicsQueue
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            m_graphicsQueueFamily = i;
        }

        // presentQueue
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
            m_presentQueueFamily = i;
        }

        i++;
    }

    return indices;

}

bool VulkanPhysicalDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device) {

    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();

}

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {

    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
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

void VulkanPhysicalDevice::CreateSupportedSampleCounts() {

    m_msaaSamples.clear();

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);

    VkSampleCountFlags counts = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;

    if (counts & VK_SAMPLE_COUNT_1_BIT) m_msaaSamples.push_back(VK_SAMPLE_COUNT_1_BIT);
    if (counts & VK_SAMPLE_COUNT_2_BIT) m_msaaSamples.push_back(VK_SAMPLE_COUNT_2_BIT);
    if (counts & VK_SAMPLE_COUNT_4_BIT) m_msaaSamples.push_back(VK_SAMPLE_COUNT_4_BIT);
    if (counts & VK_SAMPLE_COUNT_8_BIT) m_msaaSamples.push_back(VK_SAMPLE_COUNT_8_BIT);
    if (counts & VK_SAMPLE_COUNT_16_BIT) m_msaaSamples.push_back(VK_SAMPLE_COUNT_16_BIT);
    if (counts & VK_SAMPLE_COUNT_32_BIT) m_msaaSamples.push_back(VK_SAMPLE_COUNT_32_BIT);
    if (counts & VK_SAMPLE_COUNT_64_BIT) m_msaaSamples.push_back(VK_SAMPLE_COUNT_64_BIT);

    std::sort(m_msaaSamples.begin(), m_msaaSamples.end());

    auto max = m_msaaSamples.back();

    auto ToSampleCount = [](VkSampleCountFlagBits s) {
        switch (s) {
            case VK_SAMPLE_COUNT_1_BIT:  return 1;
            case VK_SAMPLE_COUNT_2_BIT:  return 2;
            case VK_SAMPLE_COUNT_4_BIT:  return 4;
            case VK_SAMPLE_COUNT_8_BIT:  return 8;
            case VK_SAMPLE_COUNT_16_BIT: return 16;
            case VK_SAMPLE_COUNT_32_BIT: return 32;
            case VK_SAMPLE_COUNT_64_BIT: return 64;
            default: return 1;
        }
    };

    if (!m_msaaSamples.empty()) {
        std::cout << "[MSAA] Max supported: " << ToSampleCount(max) << "x" << std::endl;
    } else {
        std::cout << "[MSAA] Multisampling not supported!" << std::endl;
    }

}

const std::vector<VkSampleCountFlagBits>& VulkanPhysicalDevice::GetSupportedSampleCounts() const {
    return m_msaaSamples;
}

