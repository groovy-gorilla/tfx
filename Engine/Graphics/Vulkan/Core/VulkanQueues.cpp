#include "pch.h"
#include "VulkanQueues.h"

void VulkanQueues::Create(VkDevice device, uint32_t graphicsQueueFamily, uint32_t presentQueueFamily) {

    vkGetDeviceQueue(device, graphicsQueueFamily, 0, &m_graphicsQueue);
    vkGetDeviceQueue(device, presentQueueFamily, 0, &m_presentQueue);

    std::cout << "[Vulkan] Commands created" << std::endl;

}