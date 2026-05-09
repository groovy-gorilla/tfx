#pragma once

class VulkanDevice {
public:
    void Create(VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamily, uint32_t presentQueueFamily);
    void Destroy();

    [[nodiscard]] VkDevice Get() const;

    [[nodiscard]] VkQueue GetGraphicsQueue() const;
    [[nodiscard]] VkQueue GetPresentQueue() const;

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;

};