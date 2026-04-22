#pragma once

#include <ktx.h>

#include "Vulkan.h"
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

enum class TextureType {
    Font,       // Do atlasów fontów
    Bitmap,     // Do bitmap
    Data        // Do danych nie obrazu (np. heightmapy)
};

struct TextureCreateInfo {
    TextureType type;
    bool generateMipmaps = false;
    bool enableAnisotropy = false;
    VkFilter magFilter = VK_FILTER_LINEAR;
    VkFilter minFilter = VK_FILTER_LINEAR;
    VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
};

class Texture {

public:
    Texture();
    ~Texture();

    void Initialize(VkDevice& device, VkPhysicalDevice& physicalDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, const std::string& filename, TextureType type);
    void Shutdown(VkDevice& device);

    VkImageView& GetImageView();
    VkSampler& GetSampler();

private:
    VkImage m_image;
    VkDeviceMemory m_memory;
    VkImageView m_imageView;
    VkSampler m_sampler;
    VkFormat m_format;
    uint32_t m_mipLevels;
    std::string m_filename;
    TextureCreateInfo m_createInfo{};

    void ApplyDefaults(TextureType& type);
    void ValidateTextureFormat(TextureType& type);

    void CreateBuffer(VkDevice& device, VkPhysicalDevice& physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void CreateImage(VkDevice& device, VkPhysicalDevice& physicalDevice, uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageUsageFlags usage, VkImage& image, VkDeviceMemory& memory);
    void TransitionImageLayout(VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
    void CopyBufferToImage(VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue, VkBuffer buffer, VkImage image, const std::vector<VkBufferImageCopy>& regions);
    void CreateImageView(VkDevice device, VkImage image, VkFormat format, uint32_t mipLevels, VkImageView& imageView);
    uint32_t FindMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkCommandBuffer BeginSingleTimeCommands(VkDevice& device, VkCommandPool& commandPool);
    void EndSingleTimeCommands(VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue, VkCommandBuffer commandBuffer);

};

