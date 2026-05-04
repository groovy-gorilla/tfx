#include "VulkanCommandPool.h"

#include <iostream>
#include <ostream>
#include <stdexcept>

void VulkanCommandPool::Create(VkDevice device, uint32_t graphicsQueueFamily) {

    VkCommandPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    info.queueFamilyIndex = graphicsQueueFamily;

    if (vkCreateCommandPool(device, &info, nullptr, &m_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }

    std::cout << "[Vulkan] Command pool created" << std::endl;

}

void VulkanCommandPool::Destroy(VkDevice device) {
    if (m_commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(device, m_commandPool, nullptr);
        m_commandPool = VK_NULL_HANDLE;
        std::cout << "[Vulkan] Command pool destroyed" << std::endl;
    }
}

VkCommandPool VulkanCommandPool::Get() const {
    return m_commandPool;
}