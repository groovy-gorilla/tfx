#pragma once

#include "Debug/ErrorDialog.h"

struct RenderTarget {

    void CreateSamplers(VkDevice device) {

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

        VK_CHECK(vkCreateSampler(device, &samplerNearest, nullptr, &NearestSampler));


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

        VK_CHECK(vkCreateSampler(device, &samplerLinear, nullptr, &LinearSampler));

    }

    void Destroy(VkDevice device) noexcept {

        if (NearestSampler) {
            vkDestroySampler(device, NearestSampler, nullptr);
            NearestSampler = VK_NULL_HANDLE;
        }

        if (LinearSampler) {
            vkDestroySampler(device, LinearSampler, nullptr);
            LinearSampler = VK_NULL_HANDLE;
        }

        if (View) {
            vkDestroyImageView(device, View, nullptr);
            View = VK_NULL_HANDLE;
        }

        if (Image) {
            vkDestroyImage(device, Image, nullptr);
            Image = VK_NULL_HANDLE;
        }

        if (Memory) {
            vkFreeMemory(device, Memory, nullptr);
            Memory = VK_NULL_HANDLE;
        }

    }

    VkImage Image = VK_NULL_HANDLE;
    VkImageView View = VK_NULL_HANDLE;
    VkDeviceMemory Memory = VK_NULL_HANDLE;
    VkFormat Format = VK_FORMAT_UNDEFINED;

    VkSampler NearestSampler = VK_NULL_HANDLE;
    VkSampler LinearSampler = VK_NULL_HANDLE;

    uint32_t Width = 0;
    uint32_t Height = 0;

};
