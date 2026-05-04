#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanSwapchainFramebuffers {
public:
    void Create(VkDevice device, VkRenderPass renderPass, const std::vector<VkImageView>& imageViews, VkExtent2D extent);
    void Destroy(VkDevice device);

    const std::vector<VkFramebuffer>& Get() const;

private:
    std::vector<VkFramebuffer> m_framebuffers;

};
