#pragma once

#include "../../Engine/Core/Window.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "Swapchain/VulkanSwapchain.h"
#include "VulkanImageViews.h"
#include "Offscreen/VulkanOffscreenRenderPass.h"
#include "Swapchain/VulkanSwapchainRenderPass.h"
#include "Offscreen/VulkanOffscreenFramebuffers.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffers.h"
#include "Offscreen/VulkanOffscreenPipeline.h"
#include "Offscreen/VulkanOffscreenResources.h"
#include "VulkanSampler.h"
#include "Swapchain/VulkanPostPipeline.h"

struct ViewportRect {
    float x;
    float y;
    float width;
    float height;
};


class VulkanRenderer {
public:
    void Initialize(Window& window, ApplicationDesc& desc);
    void Shutdown(ApplicationDesc& desc);

    void RecreateSwapchain(Window& window);
    ViewportRect CalculateViewport(int width, int height, const ApplicationDesc& desc);
    void RecordCommandBuffer(uint32_t imageIndex, ApplicationDesc& desc);

    void DrawFrame(Window& window, ApplicationDesc& desc);

    void CreateSyncObjects(ApplicationDesc& desc);

    void CreateDescriptorPool();
    void CreateDescriptorSet();
    void UpdateDescriptorSet();

private:
    VulkanInstance m_instance;
    VulkanSurface m_surface;
    VulkanPhysicalDevice m_physicalDevice;
    VulkanDevice m_device;
    VulkanSwapchain m_swapchain;
    VulkanImageViews m_imageViews;
    VulkanOffscreenRenderPass m_offscreenRenderPass;
    VulkanSwapchainRenderPass m_swapchainRenderPass;
    VulkanOffscreenPipeline m_offscreenPipeline;
    VulkanPostPipeline m_postPipeline;
    VulkanOffscreenFramebuffers m_framebuffers;
    VulkanCommandPool m_commandPool;
    VulkanCommandBuffers m_commandBuffers;
    VulkanOffscreenResources m_offscreenResources;
    VkDescriptorPool m_descriptorPool;
    VkDescriptorSet m_descriptorSet;
    VulkanSampler m_sampler;

    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_fences;

    size_t m_currentFrame = 0;


};

