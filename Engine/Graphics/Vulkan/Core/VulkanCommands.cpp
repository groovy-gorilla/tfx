#include "pch.h"
#include "VulkanCommands.h"

void VulkanCommands::Create(VkDevice device, uint32_t graphicsQueueFamily, uint32_t imageCount) {

    // COMMAND POOL
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsQueueFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    vkCreateCommandPool(device, &poolInfo, nullptr, &m_commandPool);

    // COMMAND BUFFERS
    m_commandBuffers.resize(imageCount);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = imageCount;

    vkAllocateCommandBuffers(device, &allocInfo, m_commandBuffers.data());

    std::cout << "[Vulkan] Commands created" << std::endl;

}

void VulkanCommands::Destroy(VkDevice device) {

    if (m_commandPool) {
        vkDestroyCommandPool(device, m_commandPool, nullptr);
        m_commandPool = VK_NULL_HANDLE;
        std::cout << "[Vulkan] Commands destroyed" << std::endl;
    }

}

void VulkanCommands::SetViewport(VkCommandBuffer commandBuffer, VkExtent2D extent) {

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

}

void VulkanCommands::SetScissor(VkCommandBuffer commandBuffer, VkExtent2D extent) {

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

}