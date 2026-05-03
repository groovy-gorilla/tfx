




class VulkanRenderTarget {
public:
    void Create(VkPhysicalDevice phys, VkDevice device, uint32_t width, uint32_t height);
    void Destroy(VkDevice device);

    VkImage GetImage() const { return m_image; }
    VkImageView GetView() const { return m_view; }
    VkFramebuffer GetFramebuffer() const { return m_framebuffer; }
    VkExtent2D GetExtent() const { return m_extent; }

private:
    VkImage m_image = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    VkImageView m_view = VK_NULL_HANDLE;
    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

    VkExtent2D m_extent{};
};

