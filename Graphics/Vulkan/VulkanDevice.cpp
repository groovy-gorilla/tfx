#include "VulkanDevice.h"

#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

#include "VulkanDebug.h"

void VulkanDevice::Create(VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamily, uint32_t presentQueueFamily) {

    std::set<uint32_t> uniqueQueues = {
        graphicsQueueFamily,
        presentQueueFamily
    };

    float queuePriority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueInfos;

    for (uint32_t queueFamily : uniqueQueues) {
        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = queueFamily;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &queuePriority;
        queueInfos.push_back(queueInfo);
    }

    VkPhysicalDeviceFeatures features{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
    createInfo.pQueueCreateInfos = queueInfos.data();
    createInfo.pEnabledFeatures = &features;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
    if (ENABLE_VALIDATION) {
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = layers;
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device");
    }

    // Pobranie kolejek
    vkGetDeviceQueue(m_device, graphicsQueueFamily, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, presentQueueFamily, 0, &m_presentQueue);


    std::cout << "[Vulkan] Logical device created" << std::endl;

}

void VulkanDevice::Destroy() {

    if (m_device != VK_NULL_HANDLE) {
        vkDestroyDevice(m_device, nullptr);
        m_device = VK_NULL_HANDLE;
        std::cout << "[Vulkan] Device destroyed" << std::endl;
    }

}

VkDevice VulkanDevice::Get() const {
    return m_device;
}

VkQueue VulkanDevice::GetGraphicsQueue() const {
    return m_graphicsQueue;
}

VkQueue VulkanDevice::GetPresentQueue() const {
    return m_presentQueue;
}
