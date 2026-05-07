#pragma once

#include <iostream>
#include <ostream>
#include <vulkan/vulkan.h>
#include "../../../Engine/Core/ApplicationDesc.h"
#include "../../../Engine/Core/Error/ErrorDialog.h"

struct RenderTarget {

    void CreateSampler(VkDevice device, TextureFilter filter) {

        VkSamplerCreateInfo sampler{};
        sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

        switch (filter) {
            case TextureFilter::Nearest:
                sampler.magFilter = VK_FILTER_NEAREST;
                sampler.minFilter = VK_FILTER_NEAREST;
                sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
                break;

            case TextureFilter::Linear:
                sampler.magFilter = VK_FILTER_LINEAR;
                sampler.minFilter = VK_FILTER_LINEAR;
                sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
                break;
        }

        sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler.anisotropyEnable = VK_FALSE;
        sampler.maxAnisotropy = 1.0f;
        sampler.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler.unnormalizedCoordinates = VK_FALSE;
        sampler.compareEnable = VK_FALSE;

        sampler.minLod = 0.0f;
        sampler.maxLod = 0.0f;
        sampler.mipLodBias = 0.0f;

        VK_CHECK(vkCreateSampler(device, &sampler, nullptr, &Sampler));

    }

    void Destroy(VkDevice device) noexcept {

        if (Sampler) {
            vkDestroySampler(device, Sampler, nullptr);
            Sampler = VK_NULL_HANDLE;
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
    VkSampler Sampler = VK_NULL_HANDLE;
    VkDeviceMemory Memory = VK_NULL_HANDLE;

    VkFormat Format = VK_FORMAT_UNDEFINED;

    uint32_t Width = 0;
    uint32_t Height = 0;

};
