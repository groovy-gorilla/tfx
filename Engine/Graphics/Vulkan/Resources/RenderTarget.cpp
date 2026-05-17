#include "RenderTarget.h"

#include "Graphics/Vulkan/Utils/VulkanUtils.h"
#include "Debug/ErrorDialog.h"

void RenderTarget::Create(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspect, VkSampleCountFlagBits samples) {

    m_width = width;
    m_height = height;
    m_format = format;

    CreateImage(device, physicalDevice, usage, samples);
    CreateImageView(device, format, aspect);
    CreateSamplers(device);

}

void RenderTarget::Destroy(VkDevice device) {

    if (m_nearestSampler) {
        vkDestroySampler(device, m_nearestSampler, nullptr);
        m_nearestSampler = VK_NULL_HANDLE;
    }

    if (m_linearSampler) {
        vkDestroySampler(device, m_linearSampler, nullptr);
        m_linearSampler = VK_NULL_HANDLE;
    }

    if (m_view) {
        vkDestroyImageView(device, m_view, nullptr);
        m_view = VK_NULL_HANDLE;
    }

    if (m_image) {
        vkDestroyImage(device, m_image, nullptr);
        m_image = VK_NULL_HANDLE;
    }

    if (m_memory) {
        vkFreeMemory(device, m_memory, nullptr);
        m_memory = VK_NULL_HANDLE;
    }

}

void RenderTarget::CreateSamplers(VkDevice device) {

    // NEAREST
    VkSamplerCreateInfo samplerNearest{};
    samplerNearest.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerNearest.magFilter = VK_FILTER_NEAREST;
    samplerNearest.minFilter = VK_FILTER_NEAREST;
    samplerNearest.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    samplerNearest.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerNearest.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerNearest.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerNearest.anisotropyEnable = VK_FALSE;
    samplerNearest.maxAnisotropy = 1.0f;
    samplerNearest.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerNearest.unnormalizedCoordinates = VK_FALSE;
    samplerNearest.compareEnable = VK_FALSE;
    samplerNearest.minLod = 0.0f;
    samplerNearest.maxLod = 0.0f;
    samplerNearest.mipLodBias = 0.0f;

    VK_CHECK(vkCreateSampler(device, &samplerNearest, nullptr, &m_nearestSampler));


    // LINEAR
    VkSamplerCreateInfo samplerLinear{};
    samplerLinear.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerLinear.magFilter = VK_FILTER_LINEAR;
    samplerLinear.minFilter = VK_FILTER_LINEAR;
    samplerLinear.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerLinear.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerLinear.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerLinear.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerLinear.anisotropyEnable = VK_FALSE;
    samplerLinear.maxAnisotropy = 1.0f;
    samplerLinear.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerLinear.unnormalizedCoordinates = VK_FALSE;
    samplerLinear.compareEnable = VK_FALSE;
    samplerLinear.minLod = 0.0f;
    samplerLinear.maxLod = 0.0f;
    samplerLinear.mipLodBias = 0.0f;

    VK_CHECK(vkCreateSampler(device, &samplerLinear, nullptr, &m_linearSampler));

}

void RenderTarget::CreateImage(VkDevice device, VkPhysicalDevice physicalDevice, VkImageUsageFlags usage, VkSampleCountFlagBits samples) {

    CreateImageResource(physicalDevice, device, m_width, m_height, m_format, usage, samples, m_image, m_memory);

}

void RenderTarget::CreateImageView(VkDevice device, VkFormat format, VkImageAspectFlags aspect) {

    CreateImageViewResource(device, m_image, format, aspect, m_view);

}