#include "pch.h"
#include "VulkanSceneResources.h"
#include "Graphics/Vulkan/Utils/VulkanUtils.h"


void VulkanSceneResources::Create(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent, VkFormat colorFormat, VkFormat depthFormat, ApplicationDesc& desc, VkRenderPass renderPass) {

    m_samples = desc.MSAA_SAMPLES;
    m_aaMode = desc.AA_MODE;

    VkExtent2D internalExtent = extent;
    VkExtent2D ssaaExtent = extent;
    ssaaExtent.width *= desc.SSAA_SCALE;
    ssaaExtent.height *= desc.SSAA_SCALE;


    // Scene Color
    CreateImage(physicalDevice, device, ssaaExtent.width, ssaaExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,VK_SAMPLE_COUNT_1_BIT,SceneColor.Image, SceneColor.Memory);
    SceneColor.Format = colorFormat;
    SceneColor.View = CreateImageView(device, SceneColor.Image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    SceneColor.CreateSamplers(device);

    // Scene Depth
    CreateImage(physicalDevice, device, ssaaExtent.width, ssaaExtent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT, SceneDepth.Image, SceneDepth.Memory);
    SceneDepth.Format = depthFormat;
    SceneDepth.View = CreateImageView(device, SceneDepth.Image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    SceneDepth.CreateSamplers(device);

    // MSAA Color
    CreateImage(physicalDevice, device, internalExtent.width, internalExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_samples, MSAAColor.Image, MSAAColor.Memory);
    MSAAColor.Format = colorFormat;
    MSAAColor.View = CreateImageView(device, MSAAColor.Image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);

    // MSAA Depth
    CreateImage(physicalDevice, device, internalExtent.width, internalExtent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, m_samples, MSAADepth.Image, MSAADepth.Memory);
    MSAADepth.Format = depthFormat;
    MSAADepth.View = CreateImageView(device, MSAADepth.Image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    // MSAA Resolve Color
    CreateImage(physicalDevice, device, internalExtent.width, internalExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT, ResolveColor.Image, ResolveColor.Memory);
    ResolveColor.Format = colorFormat;
    ResolveColor.View = CreateImageView(device, ResolveColor.Image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    ResolveColor.CreateSamplers(device);

    // SSAA Color
    CreateImage(physicalDevice, device, internalExtent.width, internalExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT, SSAAColor.Image, SSAAColor.Memory);
    SSAAColor.Format = colorFormat;
    SSAAColor.View = CreateImageView(device, SSAAColor.Image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    SSAAColor.CreateSamplers(device);

    // SMAA Color
    CreateImage(physicalDevice, device, internalExtent.width, internalExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT, SMAAColor.Image, SMAAColor.Memory);
    SMAAColor.Format = colorFormat;
    SMAAColor.View = CreateImageView(device, SMAAColor.Image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    SMAAColor.CreateSamplers(device);



    // FRAMEBUFFER
    std::vector<VkImageView> attachments;

    switch (desc.AA_MODE) {

        case AntiAliasing::MSAA:
        case AntiAliasing::MSAA_SMAA:
             attachments = {
                MSAAColor.View,
                MSAADepth.View,
                ResolveColor.View,
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
        case AntiAliasing::SSAA_SMAA:
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
    SSAAColor.Destroy(device);
    SMAAColor.Destroy(device);

    std::cout << "[Vulkan] Scene-resources destroyed" << std::endl;

}