#pragma once

struct ApplicationDesc;

class VulkanSwapchain {
public:
    void Create(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface,  VkExtent2D extent, uint32_t graphicsQueueFamily, uint32_t presentQueueFamily, ApplicationDesc& desc);
    void Destroy(VkDevice device);

    [[nodiscard]] VkSwapchainKHR Get() const { return m_swapchain; };
    [[nodiscard]] const std::vector<VkImage>& GetImages() const { return m_images; };
    [[nodiscard]] const std::vector<VkImageView>& GetImageViews() const { return m_imageViews; }
    [[nodiscard]] VkFormat GetImageFormat() const { return m_imageFormat; };
    [[nodiscard]] VkExtent2D GetExtent() const { return m_extent; };

private:
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    VkFormat m_imageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D m_extent = {};


    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& modes, ApplicationDesc& desc);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,  VkExtent2D extent);
    void CreateImageViews(VkDevice device);
    void DestroyImageViews(VkDevice device);

};
