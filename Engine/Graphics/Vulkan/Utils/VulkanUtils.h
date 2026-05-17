#pragma once

std::vector<char> ReadFile(
    const std::string& filename);

uint32_t FindMemoryType(
    VkPhysicalDevice physicalDevice,
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties);

VkFormat FindDepthFormat(
    VkPhysicalDevice physicalDevice);

void CreateImageResource(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageUsageFlags usage,
    VkSampleCountFlagBits samples,
    VkImage& image,
    VkDeviceMemory& memory);

void CreateImageViewResource(
    VkDevice device,
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspect,
    VkImageView& imageView);

VkSampler CreateSamplerResource(
    VkDevice& device);

VkShaderModule CreateShaderModule(
    VkDevice device,
    const std::vector<char>& code);

void CreateBuffer(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer& buffer,
    VkDeviceMemory& memory);

VkCommandBuffer BeginSingleTimeCommands(
    VkDevice device,
    VkCommandPool commandPool);

void EndSingleTimeCommands(
    VkDevice device,
    VkQueue queue,
    VkCommandPool commandPool,
    VkCommandBuffer commandBuffer);

void TransitionImageLayout(
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue queue,
    VkImage image,
    VkImageLayout oldLayout,
    VkImageLayout newLayout);

void CopyBufferToImage(
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue queue,
    VkBuffer buffer,
    VkImage image,
    uint32_t width,
    uint32_t height);




