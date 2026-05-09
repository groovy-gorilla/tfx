#include "pch.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image_write.h"
#include "VulkanRenderer.h"
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

    // TAKE SCREENSHOT
    if (desc.TAKE_SCREENSHOT) {
        TakeScreenshot(imageIndex);
        desc.TAKE_SCREENSHOT = false;
    }

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

void VulkanRenderer::TakeScreenshot(uint32_t imageIndex) {

    VkImage srcImage = m_swapchain.GetImages()[imageIndex];

    VkDeviceSize imageSize = static_cast<VkDeviceSize>(m_windowExtent.width) * static_cast<VkDeviceSize>(m_windowExtent.height) * 4;

    // STAGING BUFFER
    VkBuffer stagingBuffer{};
    VkDeviceMemory stagingMemory{};

    CreateBuffer(m_physicalDevice.Get(), m_device.Get(), imageSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory);

    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(m_device.Get(), m_commands.GetPool());

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = srcImage;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { m_windowExtent.width, m_windowExtent.height, 1 };

    vkCmdCopyImageToBuffer(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, stagingBuffer, 1, &region);

    VkImageMemoryBarrier barrierBack{};
    barrierBack.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierBack.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrierBack.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrierBack.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierBack.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierBack.image = srcImage;
    barrierBack.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierBack.subresourceRange.baseMipLevel = 0;
    barrierBack.subresourceRange.levelCount = 1;
    barrierBack.subresourceRange.baseArrayLayer = 0;
    barrierBack.subresourceRange.layerCount = 1;
    barrierBack.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrierBack.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierBack);

    EndSingleTimeCommands(m_device.Get(), m_queues.GetGraphics(), m_commands.GetPool(), commandBuffer);

    void* data = nullptr;

    vkMapMemory(m_device.Get(), stagingMemory, 0, imageSize, 0, &data);

    std::vector<uint8_t> pixels(imageSize);

    // KONWERSJA Z BGRA NA RGBA
    uint8_t* mapped = static_cast<uint8_t*>(data);

    for (size_t i = 0; i < imageSize; i += 4) {
        pixels[i + 0] = mapped[i + 2];
        pixels[i + 1] = mapped[i + 1];
        pixels[i + 2] = mapped[i + 0];
        pixels[i + 3] = mapped[i + 3];
    }

    // CREATE DIRECTORY
    std::filesystem::create_directory("Screenshots");

    // TIMESTAMP FILENAME
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm localtime = *std::localtime(&time);
    std::stringstream ss;
    ss << "Screenshots/screenshot_" << std::put_time(&localtime, "%Y-%m-%d_%H-%M-%S") << ".png";
    std::string filename = ss.str();

    // WRITE IMAGE TO PNG FILE
    stbi_write_png(filename.c_str(), m_windowExtent.width, m_windowExtent.height, 4, pixels.data(), m_windowExtent.width * 4);

    // DESTROY
    vkUnmapMemory(m_device.Get(), stagingMemory);
    vkDestroyBuffer(m_device.Get(), stagingBuffer, nullptr);
    vkFreeMemory(m_device.Get(), stagingMemory, nullptr);

    std::cout << "Saving screenshot: " << filename << std::endl;

}
