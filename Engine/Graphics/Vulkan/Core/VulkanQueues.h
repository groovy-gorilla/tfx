#pragma once

class VulkanQueues {
public:

    void Create(
        VkDevice device,
        uint32_t graphicsQueueFamily,
        uint32_t presentQueueFamily);

    [[nodiscard]] VkQueue GetGraphics() const { return m_graphicsQueue; }
    [[nodiscard]] VkQueue GetPresent() const { return m_presentQueue; }

private:
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;

};