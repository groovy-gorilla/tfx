#pragma once

#include <vulkan/vulkan.h>
#include "../../../Engine/Core/Window.h"
#include "../Resources/RenderTarget.h"

class VulkanSceneResources {
public:
    void Create(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent, VkFormat colorFormat, VkFormat depthFormat, ApplicationDesc& desc, VkRenderPass renderPass);
    void Destroy(VkDevice device);

    VkFramebuffer GetFramebuffer() const { return m_framebuffer; }
    VkExtent2D GetExtent() const { return m_framebufferExtent; }

    RenderTarget SceneColor{};
    RenderTarget SceneDepth{};
    RenderTarget MSAAColor{};
    RenderTarget MSAADepth{};
    RenderTarget ResolveColor{};
    RenderTarget ResolveDepth{};
    RenderTarget FinalColor{};

private:

    VkSampleCountFlagBits m_samples = VK_SAMPLE_COUNT_1_BIT;
    AntiAliasing m_aaMode = AntiAliasing::None;
    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
    VkExtent2D m_framebufferExtent{};

};