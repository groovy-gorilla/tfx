#include "Texture.h"
#include "ErrorDialog.h"
#include <cstring>
#include <iostream>
#include <ktx.h>
#include <stdexcept>
#include <vector>

Texture::Texture() {

    m_image = VK_NULL_HANDLE;
    m_memory = VK_NULL_HANDLE;
    m_imageView = VK_NULL_HANDLE;
    m_sampler = VK_NULL_HANDLE;
    m_format = VK_FORMAT_UNDEFINED;
    m_mipLevels = 1;

}

Texture::~Texture() = default;

void Texture::Initialize(VkDevice& device, VkPhysicalDevice& physicalDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, const std::string& filename, TextureType type) {

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;

    // Wczytuje plik KTX2
    ktxTexture2* ktxTex = nullptr;
    KTX_error_code result = ktxTexture2_CreateFromNamedFile(filename.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTex);
    if (result != KTX_SUCCESS) {
        throw std::runtime_error("ktxTexture2_CreateFromNamedFile failed");
    }

    if (!ktxTex) {
        throw std::runtime_error("ktxTex is null");
    }

    // Pobiera dane z pliku KTX2
    ktx_uint8_t* data = ktxTexture_GetData((ktxTexture*)ktxTex);
    VkDeviceSize imageSize = ktxTexture_GetDataSize((ktxTexture*)ktxTex);
    uint32_t width = ktxTex->baseWidth;
    uint32_t height = ktxTex->baseHeight;
    m_mipLevels = ktxTex->numLevels;
    m_format = (VkFormat)ktxTex->vkFormat;

    // Wypisuje na konsolę informacje o teksturze
    std::cout << "TEXTURE INFO" << "\n";
    std::cout << "Filename: " << filename << "\n";
    std::cout << "Width: " << width << "\n";
    std::cout << "Height: " << height << "\n";
    std::cout << "MipLevels: " << m_mipLevels << "\n";
    std::cout << "ImageSize: " << imageSize << "\n";
    std::cout << "Format: " << m_format << "\n";

    // Walidacja formatu
    ValidateTextureFormat(type);

    // Konfiguracja
    ApplyDefaults(type);

    // Staging buffer
    CreateBuffer(device, physicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory);
    void* mapped;
    VK_CHECK(vkMapMemory(device, stagingMemory, 0, imageSize, 0, &mapped));
    memcpy(mapped, data, imageSize);
    vkUnmapMemory(device, stagingMemory);

    // Tworzenie obrazu VkImage
    CreateImage(device, physicalDevice, width, height, m_mipLevels, m_format, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, m_image, m_memory);

    // Layout: UNDEFINED -> TRANSFER_DST
    TransitionImageLayout(device, commandPool, graphicsQueue, m_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mipLevels);

    // Kopiowanie mipmap
    std::vector<VkBufferImageCopy> regions;

    for (uint32_t i = 0; i < m_mipLevels; i++) {
        ktx_size_t offset;
        ktxTexture2_GetImageOffset(ktxTex, i, 0, 0, &offset);

        VkBufferImageCopy region{};
        region.bufferOffset = offset;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = i;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageExtent.width = std::max(1u, width >> i);
        region.imageExtent.height = std::max(1u, height >> i);
        region.imageExtent.depth = 1;
        regions.push_back(region);
    }

    CopyBufferToImage(device, commandPool, graphicsQueue, stagingBuffer, m_image, regions);

    // Layout: TRANSFER_DST -> SHADER_READ
    TransitionImageLayout(device, commandPool, graphicsQueue, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_mipLevels);

    // ImageView
    CreateImageView(device, m_image, m_format, m_mipLevels, m_imageView);

    // Sampler
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = m_createInfo.magFilter;
    samplerInfo.minFilter = m_createInfo.minFilter;
    samplerInfo.addressModeU = m_createInfo.addressMode;
    samplerInfo.addressModeV = m_createInfo.addressMode;
    samplerInfo.addressModeW = m_createInfo.addressMode;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(m_mipLevels);
    samplerInfo.anisotropyEnable = m_createInfo.enableAnisotropy ? VK_TRUE : VK_FALSE;
    samplerInfo.maxAnisotropy = m_createInfo.enableAnisotropy ? 16.0f : 1.0f;
    samplerInfo.mipmapMode = m_createInfo.generateMipmaps ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;

    VK_CHECK(vkCreateSampler(device, &samplerInfo, nullptr, &m_sampler));

    // Cleanup staging + KTX
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingMemory, nullptr);
    ktxTexture_Destroy((ktxTexture*)ktxTex);

}

void Texture::Shutdown(VkDevice& device) {

    if (m_sampler != VK_NULL_HANDLE) {
        vkDestroySampler(device, m_sampler, nullptr);
        m_sampler = VK_NULL_HANDLE;
    }

    if (m_imageView != VK_NULL_HANDLE) {
        vkDestroyImageView(device, m_imageView, nullptr);
        m_imageView = VK_NULL_HANDLE;
    }

    if (m_image != VK_NULL_HANDLE) {
        vkDestroyImage(device, m_image, nullptr);
        m_image = VK_NULL_HANDLE;
    }

    if (m_memory != VK_NULL_HANDLE) {
        vkFreeMemory(device, m_memory, nullptr);
        m_memory = VK_NULL_HANDLE;
    }

}

VkImageView &Texture::GetImageView() {

    return m_imageView;

}

VkSampler &Texture::GetSampler() {

    return m_sampler;

}

void Texture::ApplyDefaults(TextureType& type) {

    switch (type) {

        case TextureType::Font:
            m_createInfo.generateMipmaps = false;
            m_createInfo.enableAnisotropy = false;
            m_createInfo.magFilter = VK_FILTER_LINEAR;
            m_createInfo.minFilter = VK_FILTER_LINEAR;
            m_createInfo.addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            break;

        case TextureType::Bitmap:
            m_createInfo.generateMipmaps = false;
            m_createInfo.enableAnisotropy = false;
            m_createInfo.magFilter = VK_FILTER_LINEAR;
            m_createInfo.minFilter = VK_FILTER_LINEAR;
            m_createInfo.addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            break;

        case TextureType::Data:
            m_createInfo.generateMipmaps = false;
            m_createInfo.enableAnisotropy = false;
            m_createInfo.magFilter = VK_FILTER_NEAREST;
            m_createInfo.minFilter = VK_FILTER_NEAREST;
            m_createInfo.addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            break;
    }
}

void Texture::ValidateTextureFormat(TextureType& type) {

    switch (type) {


        case TextureType::Font:
            if (m_format != VK_FORMAT_R8_UNORM && m_format != VK_FORMAT_R8G8B8A8_UNORM) {
                throw std::runtime_error(m_filename + ": texture format not supported!");
            }
            break;
        case TextureType::Bitmap:
            if (m_format != VK_FORMAT_R8G8B8A8_SRGB && m_format != VK_FORMAT_R8G8B8A8_UNORM) {
                throw std::runtime_error(m_filename + ": texture format not supported!");
            }
            break;
        case TextureType::Data:
            if (m_format != VK_FORMAT_R8_UNORM) {
                throw std::runtime_error(m_filename + ": texture format not supported!");
            }
            break;
    }

}

void Texture::CreateBuffer(VkDevice& device, VkPhysicalDevice& physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer));

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory));

    vkBindBufferMemory(device, buffer, bufferMemory, 0);

}

void Texture::CreateImage(VkDevice& device, VkPhysicalDevice& physicalDevice, uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageUsageFlags usage, VkImage& image, VkDeviceMemory& memory) {

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateImage(device, &imageInfo, nullptr, &image));

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(
        physicalDevice,
        memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &memory));

    VK_CHECK(vkBindImageMemory(device, image, memory, 0));

}

void Texture::TransitionImageLayout(VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) {

    VkCommandBuffer cmd = BeginSingleTimeCommands(device, commandPool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags srcStage;
    VkPipelineStageFlags dstStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {

        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {

        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    }

    vkCmdPipelineBarrier(cmd, srcStage, dstStage,0,0, nullptr,0, nullptr,1, &barrier);

    EndSingleTimeCommands(device, commandPool, graphicsQueue, cmd);

}

void Texture::CopyBufferToImage(VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue, VkBuffer buffer, VkImage image, const std::vector<VkBufferImageCopy>& regions) {

    VkCommandBuffer cmd = BeginSingleTimeCommands(device, commandPool);

    vkCmdCopyBufferToImage(cmd, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(regions.size()),regions.data());

    EndSingleTimeCommands(device, commandPool, graphicsQueue, cmd);

}

void Texture::CreateImageView(VkDevice device, VkImage image, VkFormat format, uint32_t mipLevels, VkImageView& imageView) {

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;

    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(device, &viewInfo, nullptr, &imageView));

}

uint32_t Texture::FindMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {

        // sprawdzamy czy typ pamięci jest dozwolony
        if (typeFilter & (1 << i)) {

            // sprawdzamy czy ma wymagane właściwości
            if ((memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");

}

VkCommandBuffer Texture::BeginSingleTimeCommands(VkDevice& device, VkCommandPool& commandPool) {

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;

}

void Texture::EndSingleTimeCommands(VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue, VkCommandBuffer commandBuffer) {

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

}


