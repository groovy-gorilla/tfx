#pragma once

#include "Core/VulkanInstance.h"
#include "Core/VulkanDebug.h"
#include "Core/VulkanSurface.h"
#include "Core/VulkanPhysicalDevice.h"
#include "Core/VulkanDevice.h"
#include "Core/VulkanSwapchain.h"
#include "ScenePass/VulkanSceneRenderPass.h"
#include "PostProcessPass/VulkanPostRenderPass.h"
#include "ScenePass/VulkanScenePipeline.h"
#include "ScenePass/VulkanSceneResources.h"
#include "PostProcessPass/VulkanPostResources.h"
#include "Core/VulkanCommands.h"
#include "Core/VulkanSync.h"
#include "Core/VulkanQueues.h"
#include "PostProcessPass/VulkanSMAARenderPass.h"
#include "PostProcessPass/VulkanSSAARenderPass.h"

struct ViewportRect {
    float x;
    float y;
    float width;
    float height;
};


class VulkanRenderer {
public:
    void Initialize(Display& display, Window& window, ApplicationDesc& desc);
    void Shutdown(ApplicationDesc& desc);

    void RecordCommandBuffer(VkDevice device, uint32_t imageIndex, ApplicationDesc& desc);

    void Render(VkDevice device, ApplicationDesc& desc);

    void RecreateSwapchain(Display& display, Window& window, ApplicationDesc& desc);
    void RecreateRenderer(Display& display, Window& window, ApplicationDesc& desc);

    void TakeScreenshot(uint32_t imageIndex);

    VkDevice GetDevice() { return m_device.Get(); }
    VulkanSceneResources& GetSceneResources() { return m_sceneResources; }
    VulkanPostRenderPass& GetPostRenderPass() { return m_postRenderPass; }
    VulkanSSAARenderPass& GetSSAARenderPass() { return m_ssaaRenderPass; }

private:
    VulkanInstance m_instance;
    VulkanDebug m_debug;
    VulkanSurface m_surface;
    VulkanPhysicalDevice m_physicalDevice;
    VkSampleCountFlagBits m_currentMsaa = VK_SAMPLE_COUNT_1_BIT;
    VulkanDevice m_device;
    VulkanSwapchain m_swapchain;
    uint32_t m_currentFrame = 0;

    // SCENE
    VulkanSceneRenderPass m_sceneRenderPass;
    VulkanSceneResources m_sceneResources;
    VulkanScenePipeline m_scenePipeline;

    // SCREEN
    VulkanPostRenderPass m_postRenderPass;
    VulkanSMAARenderPass m_smaaRenderPass;
    VulkanSSAARenderPass m_ssaaRenderPass;
    VulkanPostResources m_postResources;

    VulkanCommands m_commands;
    VulkanSync m_sync;
    VulkanQueues m_queues;

    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

    // EXTENT
    VkExtent2D m_renderExtent = {};
    VkExtent2D m_windowExtent = {};



};
