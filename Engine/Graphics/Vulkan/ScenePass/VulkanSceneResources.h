#pragma once

#include "Core/Window.h"
#include "Graphics/Vulkan/Resources/RenderTarget.h"

class VulkanSceneResources {
public:
    void Create(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent, VkFormat colorFormat, VkFormat depthFormat, ApplicationDesc& desc, VkRenderPass renderPass);
    void Destroy(VkDevice device);

    [[nodiscard]] VkFramebuffer GetFramebuffer() const { return m_framebuffer; }
    [[nodiscard]] VkExtent2D GetExtent() const { return m_framebufferExtent; }

    RenderTarget SceneColor;      // Normal rendering    --> Surowy obraz gry
    RenderTarget SceneDepth;      // Depth attachment    --> Bufor głębokości sceny. Jak daleko od kamery jest piksel. Depth test, ukrywanie obiektów, kolejność geometrii.

    RenderTarget MSAAColor;       // Multisampled color  --> Multisampled color buffer. Każdy piksel ma kilka próbek koloru.
    RenderTarget MSAADepth;       // Multisampled depth  --> Multisampled depth buffer dla MSAA
    RenderTarget ResolveColor;    // Resolved MSAA       --> Finalny obraz po MSAA

    RenderTarget SSAAColor;      // SSAA Output         --> Finalny obraz po SSAA

    RenderTarget SMAAColor;

private:

    VkSampleCountFlagBits m_samples = VK_SAMPLE_COUNT_1_BIT;
    AntiAliasing m_aaMode = AntiAliasing::None;
    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
    VkExtent2D m_framebufferExtent{};

};