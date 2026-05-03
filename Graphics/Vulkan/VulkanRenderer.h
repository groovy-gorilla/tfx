#pragma once

#include "../../Engine/Core/Window.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanImageViews.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffers.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffers.h"
#include "VulkanPipeline.h"

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

    void RecreateSwapchain(Window& window, ApplicationDesc& desc);
    ViewportRect CalculateViewport(int width, int height, const ApplicationDesc& desc);
    void RecordCommandBuffer(uint32_t imageIndex, ApplicationDesc& desc);

    void DrawFrame(Window& window, ApplicationDesc& desc);

private:
    VulkanInstance m_instance;
    VulkanSurface m_surface;
    VulkanPhysicalDevice m_physicalDevice;
    VulkanDevice m_device;
    VulkanSwapchain m_swapchain;
    VulkanImageViews m_imageViews;
    VulkanRenderPass m_renderPass;
    VulkanFramebuffers m_framebuffers;
    VulkanCommandPool m_commandPool;
    VulkanCommandBuffers m_commandBuffers;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_fences;
    VulkanPipeline m_pipeline;

    size_t m_currentFrame = 0;


};

