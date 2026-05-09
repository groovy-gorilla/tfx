#include "VulkanSceneResources.h"

#include <iostream>
#include <ostream>

#include "../../../Engine/Core/Error/ErrorDialog.h"
#include "../Utils/VulkanUtils.h"


void VulkanSceneResources::Create(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent, VkFormat colorFormat, VkFormat depthFormat, ApplicationDesc& desc, VkRenderPass renderPass) {

    m_samples = desc.MSAA_SAMPLES;
    m_aaMode = desc.AA_MODE;

    VkExtent2D internalExtent = extent;
    VkExtent2D ssaaExtent = extent;
    ssaaExtent.width *= desc.SSAA_SCALE;
    ssaaExtent.height *= desc.SSAA_SCALE;


    // Scene Color
    CreateImage(physicalDevice, device, ssaaExtent.width, ssaaExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,VK_SAMPLE_COUNT_1_BIT,SceneColor.Image, SceneColor.Memory);
    SceneColor.View = CreateImageView(device, SceneColor.Image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    SceneColor.CreateSamplers(device);

    // Final Color
    CreateImage(physicalDevice, device, internalExtent.width, internalExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT, FinalColor.Image, FinalColor.Memory);
    FinalColor.View = CreateImageView(device, FinalColor.Image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    FinalColor.CreateSamplers(device);

    // MSAA Color
    CreateImage(physicalDevice, device, internalExtent.width, internalExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_samples, MSAAColor.Image, MSAAColor.Memory);
    MSAAColor.View = CreateImageView(device, MSAAColor.Image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);

    // Resolve Color
    CreateImage(physicalDevice, device, internalExtent.width, internalExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT, ResolveColor.Image, ResolveColor.Memory);
    ResolveColor.View = CreateImageView(device, ResolveColor.Image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    ResolveColor.CreateSamplers(device);

    // Scene Depth
    CreateImage(physicalDevice, device, ssaaExtent.width, ssaaExtent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT, SceneDepth.Image, SceneDepth.Memory);
    SceneDepth.View = CreateImageView(device, SceneDepth.Image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    SceneDepth.CreateSamplers(device);

    // MSAA Depth
    CreateImage(physicalDevice, device, internalExtent.width, internalExtent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, m_samples, MSAADepth.Image, MSAADepth.Memory);
    MSAADepth.View = CreateImageView(device, MSAADepth.Image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    // Resolve Depth
    CreateImage(physicalDevice, device, internalExtent.width, internalExtent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT, ResolveDepth.Image, ResolveDepth.Memory);
    ResolveDepth.View = CreateImageView(device, ResolveDepth.Image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    ResolveDepth.CreateSamplers(device);



    // FRAMEBUFFER
    std::vector<VkImageView> attachments;

    switch (desc.AA_MODE) {

        case AntiAliasing::MSAA:
        case AntiAliasing::MSAA_TAA:
             attachments = {
                MSAAColor.View,
                MSAADepth.View,
                ResolveColor.View,
                ResolveDepth.View
             };
             break;

        default:
             attachments = {
                SceneColor.View,
                SceneDepth.View
             };

    }

    switch (desc.AA_MODE) {
        case AntiAliasing::SSAA:
        case AntiAliasing::SSAA_TAA:
            m_framebufferExtent = ssaaExtent;
            break;

        default:
            m_framebufferExtent = internalExtent;
    }

    VkFramebufferCreateInfo fb{};
    fb.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb.renderPass = renderPass;
    fb.attachmentCount = static_cast<uint32_t>(attachments.size());
    fb.pAttachments = attachments.data();
    fb.width = m_framebufferExtent.width;
    fb.height = m_framebufferExtent.height;
    fb.layers = 1;

    VK_CHECK(vkCreateFramebuffer(device, &fb, nullptr, &m_framebuffer));

    std::cout << "[Vulkan] Scene-resources created" << std::endl;

}

void VulkanSceneResources::Destroy(VkDevice device) {

    vkDestroyFramebuffer(device, m_framebuffer, nullptr);
    m_framebuffer = VK_NULL_HANDLE;

    SceneColor.Destroy(device);
    SceneDepth.Destroy(device);
    MSAAColor.Destroy(device);
    MSAADepth.Destroy(device);
    ResolveColor.Destroy(device);
    ResolveDepth.Destroy(device);
    FinalColor.Destroy(device);

    std::cout << "[Vulkan] Scene-resources destroyed" << std::endl;

}
