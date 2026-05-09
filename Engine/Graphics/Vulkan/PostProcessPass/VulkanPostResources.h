#pragma once

class VulkanPostResources {
public:

    void Create(
        VkDevice device,
        VkRenderPass renderPass,
        VkExtent2D extent,
        const std::vector<VkImageView>& imageViews);

    void Destroy(VkDevice device);

    [[nodiscard]] VkFramebuffer GetFramebuffer(uint32_t index) const { return m_framebuffers[index]; }

private:
    std::vector<VkFramebuffer> m_framebuffers;

};