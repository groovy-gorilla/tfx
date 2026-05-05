#include "VulkanSceneResources.h"

#include <array>
#include <iostream>
#include <ostream>

#include "../../../Engine/Core/Error/ErrorDialog.h"
#include "../VulkanUtils.h"


void VulkanSceneResources::Create(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent, VkFormat colorFormat, VkFormat depthFormat, VkSampleCountFlagBits samples, VkRenderPass renderPass) {

    m_extent = extent;
    m_samples = samples;

    // COLOR
    if (samples == VK_SAMPLE_COUNT_1_BIT) {

        CreateImage(physicalDevice, device, extent.width, extent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,VK_SAMPLE_COUNT_1_BIT,m_colorImage, m_colorMemory);
        m_colorImageView = CreateImageView(device, m_colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);

    } else {

        // MSAA color
        CreateImage(physicalDevice, device, extent.width, extent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, samples, m_msaaImage, m_msaaMemory);
        m_msaaImageView = CreateImageView(device, m_msaaImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);

        // resolve
        CreateImage(physicalDevice, device, extent.width, extent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT, m_resolveImage, m_resolveMemory);
        m_resolveImageView = CreateImageView(device, m_resolveImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);

    }

    // DEPTH
    CreateImage(physicalDevice, device, extent.width, extent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, samples, m_depthImage, m_depthMemory);
    m_depthImageView = CreateImageView(device, m_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    // FRAMEBUFFER
    std::vector<VkImageView> attachments;

    if (samples == VK_SAMPLE_COUNT_1_BIT) {

        attachments = {
            m_colorImageView,
            m_depthImageView
        };

    } else {

        attachments = {
            m_msaaImageView,
            m_depthImageView,
            m_resolveImageView
        };

    }

    VkFramebufferCreateInfo fb{};
    fb.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb.renderPass = renderPass;
    fb.attachmentCount = static_cast<uint32_t>(attachments.size());
    fb.pAttachments = attachments.data();
    fb.width = extent.width;
    fb.height = extent.height;
    fb.layers = 1;

    VK_CHECK(vkCreateFramebuffer(device, &fb, nullptr, &m_framebuffer));

    std::cout << "[Vulkan] Scene resources created" << std::endl;

}

void VulkanSceneResources::Destroy(VkDevice device) {

    vkDestroyFramebuffer(device, m_framebuffer, nullptr);

    vkDestroyImageView(device, m_depthImageView, nullptr);
    vkDestroyImage(device, m_depthImage, nullptr);
    vkFreeMemory(device, m_depthMemory, nullptr);

    if (m_samples == VK_SAMPLE_COUNT_1_BIT) {

        vkDestroyImageView(device, m_colorImageView, nullptr);
        vkDestroyImage(device, m_colorImage, nullptr);
        vkFreeMemory(device, m_colorMemory, nullptr);

    } else {

        vkDestroyImageView(device, m_msaaImageView, nullptr);
        vkDestroyImage(device, m_msaaImage, nullptr);
        vkFreeMemory(device, m_msaaMemory, nullptr);

        vkDestroyImageView(device, m_resolveImageView, nullptr);
        vkDestroyImage(device, m_resolveImage, nullptr);
        vkFreeMemory(device, m_resolveMemory, nullptr);

    }

    std::cout << "[Vulkan] Scene resources destroyed" << std::endl;

}

void VulkanSceneResources::CreateImage(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits samples, VkImage& image, VkDeviceMemory& memory) {

    VkImageCreateInfo img{};
    img.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    img.imageType = VK_IMAGE_TYPE_2D;
    img.extent = { width, height, 1 };
    img.mipLevels = 1;
    img.arrayLayers = 1;
    img.format = format;
    img.tiling = VK_IMAGE_TILING_OPTIMAL;
    img.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    img.usage = usage;
    img.samples = samples;
    img.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateImage(device, &img, nullptr, &image);

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(device, image, &memReq);

    VkMemoryAllocateInfo alloc{};
    alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc.allocationSize = memReq.size;
    alloc.memoryTypeIndex = FindMemoryType(
        physicalDevice,
        memReq.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vkAllocateMemory(device, &alloc, nullptr, &memory);
    vkBindImageMemory(device, image, memory, 0);

}

VkImageView VulkanSceneResources::CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect) {

    VkImageViewCreateInfo view{};
    view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view.image = image;
    view.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view.format = format;
    view.subresourceRange.aspectMask = aspect;
    view.subresourceRange.levelCount = 1;
    view.subresourceRange.layerCount = 1;

    VkImageView imageView;
    vkCreateImageView(device, &view, nullptr, &imageView);
    return imageView;

}


