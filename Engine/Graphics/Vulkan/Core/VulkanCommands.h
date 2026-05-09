#pragma once

class VulkanCommands {
public:

    void Create(
        VkDevice device,
        uint32_t graphicsQueueFamily,
        uint32_t imageCount);

    void Destroy(VkDevice device);

    void SetViewport(VkCommandBuffer commandBuffer, VkExtent2D extent);
    void SetScissor(VkCommandBuffer commandBuffer, VkExtent2D extent);

    [[nodiscard]] VkCommandBuffer Get(uint32_t index) const { return m_commandBuffers[index]; }
    [[nodiscard]] VkCommandPool GetPool() const { return m_commandPool; }

private:

    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_commandBuffers;

};
