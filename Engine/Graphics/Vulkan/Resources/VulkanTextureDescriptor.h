#pragma once

enum class TextureFilter;
class RenderTarget;

class VulkanTextureDescriptor {
public:

    void Create(VkDevice device, RenderTarget& colorTarget, RenderTarget& depthTarget, TextureFilter filter);
    void Destroy(VkDevice device);

    [[nodiscard]] VkDescriptorSetLayout GetLayout() const { return m_layout; }
    [[nodiscard]] VkDescriptorSet GetSet() const { return m_set; }

    void UpdateSampler(
        VkDevice device,
        RenderTarget& color,
        TextureFilter filter);

private:
    VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
    VkDescriptorPool m_pool = VK_NULL_HANDLE;
    VkDescriptorSet m_set = VK_NULL_HANDLE;

};