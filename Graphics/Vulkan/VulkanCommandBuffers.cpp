#include "VulkanCommandBuffers.h"

#include <iostream>
#include <ostream>
#include <stdexcept>

void VulkanCommandBuffers::Create(VkDevice device, VkCommandPool pool, uint32_t count) {


    m_commandBuffers.resize(count);

    VkCommandBufferAllocateInfo alloc{};
    alloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc.commandPool = pool;
    alloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc.commandBufferCount = count;

    if (vkAllocateCommandBuffers(device, &alloc, m_commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers");
    }

    std::cout << "[Vulkan] Command buffers created" << std::endl;

}

void VulkanCommandBuffers::Destroy(VkDevice device, VkCommandPool pool) {
    vkFreeCommandBuffers(device, pool, static_cast<uint32_t>(m_commandBuffers.size()),m_commandBuffers.data());
    std::cout << "[Vulkan] Command buffers destroyed" << std::endl;
}

const std::vector<VkCommandBuffer>& VulkanCommandBuffers::Get() const {
    return m_commandBuffers;
}
