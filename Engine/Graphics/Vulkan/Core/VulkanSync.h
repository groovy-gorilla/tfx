#pragma once

class VulkanSync {
public:

    void Create(VkDevice device, uint32_t maxFramesInFlight);
    void Destroy(VkDevice device);
    void Wait(VkDevice device);
    void Reset(VkDevice device);

    [[nodiscard]] VkSemaphore GetImageAvailableSemaphore() const;
    [[nodiscard]] VkSemaphore GetRenderFinishedSemaphore() const;
    [[nodiscard]] VkFence GetFence() const;

    [[nodiscard]] uint32_t GetCurrentFrame() const;

    void NextFrame(uint32_t maxFrames);

private:
    std::vector<VkFence> m_renderFences{};
    std::vector<VkSemaphore> m_imageAvailableSemaphores{};
    std::vector<VkSemaphore> m_renderFinishedSemaphores{};

    uint32_t m_currentFrame = 0;

};