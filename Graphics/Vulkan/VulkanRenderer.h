#pragma once

#include "../../Engine/Core/Window.h"
#include "VulkanInstance.h"
#include "VulkanDebug.h"
#include "VulkanSurface.h"


#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "Scene/VulkanSceneRenderPass.h"
#include "Screen/VulkanScreenRenderPass.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffers.h"
#include "Scene/VulkanScenePipeline.h"
#include "Scene/VulkanSceneResources.h"
#include "VulkanSampler.h"

struct ViewportRect {
    float x;
    float y;
    float width;
    float height;
};


class VulkanRenderer {
public:
    void Initialize(Window& window, ApplicationDesc& desc);
    void Shutdown();

    void SetMSAA(VkSampleCountFlagBits msaa);




// TEGO JESZCZE NIE MA:
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
    VulkanDebug m_debug;
    VulkanSurface m_surface;
    VulkanPhysicalDevice m_physicalDevice;
    VkSampleCountFlagBits m_currentMsaa = VK_SAMPLE_COUNT_1_BIT;
    VulkanDevice m_device;
    VulkanSwapchain m_swapchain;

    // SCENE
    VulkanSceneRenderPass m_sceneRenderPass;
    VulkanSceneResources m_sceneResources;
    VulkanScenePipeline m_scenePipeline;

    // SCREEN
    VulkanScreenRenderPass m_screenRenderPass;
    //VulkanScreenResources m_screenResources;
    //VulkanScreenPipeline m_screenPipeline;




    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;

    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;








// TEGO JESZCZE NIE MA:
    VulkanCommandPool m_commandPool;
    VulkanCommandBuffers m_commandBuffers;

    VkDescriptorPool m_descriptorPool;
    VkDescriptorSet m_descriptorSet;
    VulkanSampler m_sampler;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_fences;
    size_t m_currentFrame = 0;




};

