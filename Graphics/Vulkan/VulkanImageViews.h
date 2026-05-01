#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanImageViews {
public:
    void Create(VkDevice device, const std::vector<VkImage>& images, VkFormat format);
    void Destroy(VkDevice device);

    const std::vector<VkImageView>& Get() const;

private:
    std::vector<VkImageView> m_imageViews;

};

