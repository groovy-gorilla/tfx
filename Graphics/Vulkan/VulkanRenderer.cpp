#include "VulkanRenderer.h"

#include <iostream>
#include <ostream>

#include "Core/VulkanDebug.h"
#include "../../Engine/Core/Window.h"
#include "../../Engine/Core/Error/ErrorDialog.h"
#include "Utils/VulkanUtils.h"

void VulkanRenderer::Initialize(Display& display, Window& window, ApplicationDesc& desc) {

    // ROZDZIELCZOŚĆ RENDERINGU
    m_renderExtent = window.GetRenderExtent(desc);

    // ROZDZIELCZOŚĆ OKNA
    m_windowExtent = window.GetWindowExtent(display, desc);

    std::cout << "Rendering resolution: " << m_renderExtent.width << "x" << m_renderExtent.height << std::endl;
    std::cout << "Window resolution: " << m_windowExtent.width << "x" << m_windowExtent.height << std::endl;

    // CORE
    m_instance.Create(desc);
    m_debug.Create(m_instance.Get());
    m_surface.Create(m_instance.Get(), window.GetHandle());
    m_physicalDevice.Pick(m_instance.Get(), m_surface.Get());
    m_physicalDevice.CreateSupportedSampleCounts();
    m_device.Create(m_physicalDevice.Get(), m_physicalDevice.GetGraphicsQueueFamily(), m_physicalDevice.GetPresentQueueFamily());

    m_queues.Create(m_device.Get(), m_physicalDevice.GetGraphicsQueueFamily(), m_physicalDevice.GetPresentQueueFamily());
    m_swapchain.Create(m_physicalDevice.Get(), m_device.Get(), m_surface.Get(), m_windowExtent, m_physicalDevice.GetGraphicsQueueFamily(), m_physicalDevice.GetPresentQueueFamily(), desc);
    m_commands.Create(m_device.Get(), m_physicalDevice.GetGraphicsQueueFamily(), static_cast<uint32_t>(m_swapchain.GetImages().size()));
    m_sync.Create(m_device.Get(), desc.MAX_FRAMES_IN_FLIGHT);

    // SCENE
    m_sceneRenderPass.Create(m_device.Get(), m_swapchain.GetImageFormat(), FindDepthFormat(m_physicalDevice.Get()), desc.AA_MODE, desc.MSAA_SAMPLES);
    m_sceneResources.Create(m_physicalDevice.Get(), m_device.Get(), m_renderExtent, m_swapchain.GetImageFormat(), FindDepthFormat(m_physicalDevice.Get()), desc, m_sceneRenderPass.Get());
    m_scenePipeline.Create(m_device.Get(), m_sceneRenderPass.Get(), desc.AA_MODE, desc.MSAA_SAMPLES);

    // POST
    RenderTarget* postColor = &m_sceneResources.SceneColor;
    switch (desc.AA_MODE) {
        case AntiAliasing::MSAA:
        case AntiAliasing::MSAA_TAA:
            postColor = &m_sceneResources.ResolveColor;
            break;
        case AntiAliasing::SSAA:
        case AntiAliasing::SSAA_TAA:
            postColor = &m_sceneResources.FinalColor;
            break;
        default:
            break;
    }
    m_ssaaRenderPass.Create(m_device.Get(), m_renderExtent, m_swapchain.GetImageFormat(), m_sceneResources.SceneColor, m_sceneResources.SceneDepth, m_sceneResources.FinalColor, desc);
    m_postRenderPass.Create(m_device.Get(), m_windowExtent, m_swapchain.GetImageFormat(), *postColor, m_sceneResources.ResolveDepth, desc);
    m_postResources.Create(m_device.Get(), m_postRenderPass.Get(), m_windowExtent, m_swapchain.GetImageViews());

}

void VulkanRenderer::Shutdown(ApplicationDesc& desc) {

    vkDeviceWaitIdle(m_device.Get());

    m_postResources.Destroy(m_device.Get());
    m_postRenderPass.Destroy(m_device.Get());
    m_ssaaRenderPass.Destroy(m_device.Get());
    m_scenePipeline.Destroy(m_device.Get());
    m_sceneResources.Destroy(m_device.Get());
    m_sceneRenderPass.Destroy(m_device.Get());
    m_sync.Destroy(m_device.Get());
    m_commands.Destroy(m_device.Get());
    m_swapchain.Destroy(m_device.Get());
    m_device.Destroy();
    m_surface.Destroy(m_instance.Get());
    m_debug.Destroy(m_instance.Get());
    m_instance.Destroy();

}


void VulkanRenderer::RecordCommandBuffer(uint32_t imageIndex, ApplicationDesc& desc) {

    VkCommandBuffer commandBuffer = m_commands.Get(imageIndex);

    // RESET
    vkResetCommandBuffer(commandBuffer, 0);

    // BEGIN COMMAND BUFFER
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    // SCENE PASS
    m_sceneRenderPass.Begin(commandBuffer, m_sceneResources.GetFramebuffer(), m_sceneResources.GetExtent());
    m_scenePipeline.Bind(commandBuffer);

    m_commands.SetViewport(commandBuffer, m_sceneResources.GetExtent());
    m_commands.SetScissor(commandBuffer, m_sceneResources.GetExtent());

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    m_sceneRenderPass.End(commandBuffer);

    // SSAA PASS
    if (desc.AA_MODE == AntiAliasing::SSAA || desc.AA_MODE == AntiAliasing::SSAA_TAA) {
        m_ssaaRenderPass.Render(commandBuffer, m_renderExtent);
    }

    // POST PASS
    m_postRenderPass.Render(commandBuffer, m_postResources.GetFramebuffer(imageIndex), m_swapchain.GetExtent(), desc);

    // END COMMAND BUFFER
    vkEndCommandBuffer(commandBuffer);

}

void VulkanRenderer::Render(ApplicationDesc& desc) {

    m_sync.Wait(m_device.Get());

    // ACQUIRE IMAGE
    uint32_t imageIndex = 0;
    VK_CHECK(vkAcquireNextImageKHR(m_device.Get(), m_swapchain.Get(), UINT64_MAX, m_sync.GetImageAvailableSemaphore(), VK_NULL_HANDLE, &imageIndex));

    // RECORD COMMANDS
    RecordCommandBuffer(imageIndex, desc);

    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    m_sync.Reset(m_device.Get());

    // SUBMIT
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    VkCommandBuffer commandBuffer = m_commands.Get(imageIndex);
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.waitSemaphoreCount = 1;
    VkSemaphore availableSemaphore = m_sync.GetImageAvailableSemaphore();
    submitInfo.pWaitSemaphores = &availableSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.signalSemaphoreCount = 1;
    VkSemaphore finishedSemaphore = m_sync.GetRenderFinishedSemaphore();
    submitInfo.pSignalSemaphores = &finishedSemaphore;

    VK_CHECK(vkQueueSubmit(m_queues.GetGraphics(), 1, &submitInfo, m_sync.GetFence()));

    // PRESENT
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    VkSwapchainKHR swapchains[] = {
        m_swapchain.Get()
    };

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &finishedSemaphore;

    VK_CHECK(vkQueuePresentKHR(m_queues.GetPresent(), &presentInfo));

    m_sync.NextFrame(desc.MAX_FRAMES_IN_FLIGHT);

}

void VulkanRenderer::RecreateSwapchain(Display& display, Window& window, ApplicationDesc& desc) {

    vkDeviceWaitIdle(m_device.Get());

    // NOWE WINDOW EXTENT
    m_windowExtent = window.GetWindowExtent(display, desc);

    // DESTROY
    m_postResources.Destroy(m_device.Get());
    m_swapchain.Destroy(m_device.Get());

    // CREATE
    m_swapchain.Create(m_physicalDevice.Get(), m_device.Get(), m_surface.Get(), m_windowExtent, m_physicalDevice.GetGraphicsQueueFamily(), m_physicalDevice.GetPresentQueueFamily(),desc);

    m_postResources.Create(m_device.Get(), m_postRenderPass.Get(), m_windowExtent, m_swapchain.GetImageViews());

}

void VulkanRenderer::RecreateRenderer(Display& display, Window& window, ApplicationDesc& desc) {

    vkDeviceWaitIdle(m_device.Get());

    // DESTROY
    m_postResources.Destroy(m_device.Get());
    m_postRenderPass.Destroy(m_device.Get());
    m_ssaaRenderPass.Destroy(m_device.Get());
    m_scenePipeline.Destroy(m_device.Get());
    m_sceneResources.Destroy(m_device.Get());
    m_sceneRenderPass.Destroy(m_device.Get());
    m_swapchain.Destroy(m_device.Get());

    // EXTENTS
    m_renderExtent = window.GetRenderExtent(desc);
    m_windowExtent = window.GetWindowExtent(display, desc);

    // SWAPCHAIN
    m_swapchain.Create(m_physicalDevice.Get(), m_device.Get(), m_surface.Get(), m_windowExtent, m_physicalDevice.GetGraphicsQueueFamily(), m_physicalDevice.GetPresentQueueFamily(), desc);

    // SCENE
    m_sceneRenderPass.Create(m_device.Get(), m_swapchain.GetImageFormat(), FindDepthFormat(m_physicalDevice.Get()), desc.AA_MODE, desc.MSAA_SAMPLES);
    m_sceneResources.Create(m_physicalDevice.Get(), m_device.Get(), m_renderExtent, m_swapchain.GetImageFormat(), FindDepthFormat(m_physicalDevice.Get()), desc, m_sceneRenderPass.Get());
    m_scenePipeline.Create(m_device.Get(), m_sceneRenderPass.Get(), desc.AA_MODE, desc.MSAA_SAMPLES);

    // SSAA
    m_ssaaRenderPass.Create(m_device.Get(), m_renderExtent, m_swapchain.GetImageFormat(), m_sceneResources.SceneColor, m_sceneResources.SceneDepth, m_sceneResources.FinalColor, desc);

    // POST TARGET
    RenderTarget* postTarget = nullptr;

    switch (desc.AA_MODE) {
        case AntiAliasing::SSAA:
        case AntiAliasing::SSAA_TAA:
            postTarget = &m_sceneResources.FinalColor;
            break;
        case AntiAliasing::MSAA:
        case AntiAliasing::MSAA_TAA:
            postTarget = &m_sceneResources.ResolveColor;
            break;
        default:
            postTarget = &m_sceneResources.SceneColor;
            break;
    }

    // POST
    m_postRenderPass.Create(m_device.Get(), m_windowExtent, m_swapchain.GetImageFormat(), *postTarget, m_sceneResources.ResolveDepth, desc);
    m_postResources.Create(m_device.Get(), m_postRenderPass.Get(), m_windowExtent, m_swapchain.GetImageViews());

}


