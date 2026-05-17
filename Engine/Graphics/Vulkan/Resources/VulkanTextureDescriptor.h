#pragma once

enum class TextureFilter;
class RenderTarget;

class VulkanTextureDescriptor {
public:

    void Create(VkDevice device,  uint32_t maxFramesInFlight, RenderTarget& colorTarget, RenderTarget& depthTarget, TextureFilter filter);
    void Create(VkDevice device, uint32_t maxFramesInFlight, TextureFilter filter);
    void CreateColor(VkDevice device, uint32_t maxFramesInFlight);
    void CreateSMAABlend(VkDevice device, uint32_t maxFramesInFlight);
    void CreateSMAANeighborhood(VkDevice device, uint32_t maxFramesInFlight);

    void Destroy(VkDevice device);

    [[nodiscard]] VkDescriptorSetLayout GetLayout() const { return m_layout; }
    [[nodiscard]] VkDescriptorSet GetSet(uint32_t frameIndex) const { return m_sets[frameIndex]; }

    void CreateDescriptorResources(VkDevice device, uint32_t maxFramesInFlight);

    void UpdateColor(
        VkDevice device,
        uint32_t frameIndex,
        RenderTarget& color,
        TextureFilter filter);

    void UpdateDepth(
        VkDevice device,
        uint32_t frameIndex,
        RenderTarget& depth,
        TextureFilter filter);

    void UpdateSMAABlend(
        VkDevice device,
        uint32_t frameIndex,
        RenderTarget& edge,
        RenderTarget& area,
        RenderTarget& search);

    void UpdateSMAANeighborhood(
        VkDevice device,
        uint32_t currentFrame,
        RenderTarget& inputColor,
        RenderTarget& blendWeights);

private:
    VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
    VkDescriptorPool m_pool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_sets;


};