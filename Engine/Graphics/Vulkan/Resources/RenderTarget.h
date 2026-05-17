#pragma once

class RenderTarget {

public:
    void Create(VkDevice device,
        VkPhysicalDevice physicalDevice,
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageUsageFlags usage,
        VkImageAspectFlags aspect,
        VkSampleCountFlagBits samples);

    void Destroy(VkDevice device);

    VkImage GetImage() { return m_image; }
    VkImageView GetImageView() { return m_view; }
    VkFormat GetFormat() { return m_format; }
    VkSampler GetNearestSampler() { return m_nearestSampler; }
    VkSampler GetLinearSampler() { return m_linearSampler; }
    uint32_t GetWidth() { return m_width; }
    uint32_t GetHeight() { return m_height; }

private:
    VkImage m_image = VK_NULL_HANDLE;
    VkImageView m_view = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    VkFormat m_format = VK_FORMAT_UNDEFINED;

    VkSampler m_nearestSampler = VK_NULL_HANDLE;
    VkSampler m_linearSampler = VK_NULL_HANDLE;

    uint32_t m_width = 0;
    uint32_t m_height = 0;

    void CreateSamplers(VkDevice device);
    void CreateImage(VkDevice device, VkPhysicalDevice physicalDevice, VkImageUsageFlags usage, VkSampleCountFlagBits samples);
    void CreateImageView(VkDevice device, VkFormat format, VkImageAspectFlags aspect);

};
