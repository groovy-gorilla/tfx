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

class VulkanRenderer {
public:
    void Initialize(Window& window);
    void Shutdown();

    void RecreateSwapchain(Window& window);
    void RecordCommandBuffer(uint32_t imageIndex);

    void DrawFrame(Window& window);

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
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    VkSemaphore m_imageAvailableSemaphore;
    VkSemaphore m_renderFinishedSemaphore;
    VkFence m_fence;



};

