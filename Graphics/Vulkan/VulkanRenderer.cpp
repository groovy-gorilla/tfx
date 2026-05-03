#include "VulkanRenderer.h"

#include <iostream>
#include <stdexcept>
#include "../../Engine/Core/Window.h"

void VulkanRenderer::Initialize(Window& window, ApplicationDesc& desc) {

    m_instance.Create();
    m_surface.Create(m_instance.Get(), window.GetHandle());
    m_physicalDevice.Pick(m_instance.Get(), m_surface.Get());
    m_device.Create(m_physicalDevice.Get(), m_physicalDevice.GetGraphicsQueueFamily(), m_physicalDevice.GetPresentQueueFamily());

    int width, height;
    window.GetFramebufferSize(width, height);

    m_swapchain.Create(m_physicalDevice.Get(), m_device.Get(), m_surface.Get(), width, height, m_physicalDevice.GetGraphicsQueueFamily(), m_physicalDevice.GetPresentQueueFamily());
    m_imageViews.Create(m_device.Get(), m_swapchain.GetImages(), m_swapchain.GetImageFormat());

    // Create offscreen resources
    // create msaa resources
    // create offscreen render pass
    // create msaa render pass


    m_renderPass.Create(m_device.Get(), m_swapchain.GetImageFormat()); // to usunąć

    m_framebuffers.Create(m_device.Get(), m_renderPass.Get(), m_imageViews.Get(), m_swapchain.GetExtent());



    m_commandPool.Create(m_device.Get(), m_physicalDevice.GetGraphicsQueueFamily());
    m_commandBuffers.Create(m_device.Get(), m_commandPool.Get(), static_cast<uint32_t>(m_framebuffers.Get().size()));

    m_imageAvailableSemaphores.resize(desc.maxFramesInFlight);
    m_renderFinishedSemaphores.resize(desc.maxFramesInFlight);
    m_fences.resize(desc.maxFramesInFlight);

    VkSemaphoreCreateInfo semInfo{};
    semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < desc.maxFramesInFlight; i++) {
        if (vkCreateSemaphore(m_device.Get(), &semInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_device.Get(), &semInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_device.Get(), &fenceInfo, nullptr, &m_fences[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create sync objects");
            }
    }

    m_graphicsQueue = m_device.GetGraphicsQueue();
    m_presentQueue  = m_device.GetPresentQueue();

    m_pipeline.Create(m_device.Get(), m_swapchain.GetExtent(), m_renderPass.Get()
    );

}

void VulkanRenderer::RecreateSwapchain(Window& window, ApplicationDesc& desc) {

    int width = 0, height = 0;

    while (width == 0 || height == 0) {
        window.GetFramebufferSize(width, height);
        SDL_WaitEvent(nullptr);
    }

    vkDeviceWaitIdle(m_device.Get());

    // Destroy
    m_commandBuffers.Destroy(m_device.Get(), m_commandPool.Get());
    m_framebuffers.Destroy(m_device.Get());
    m_imageViews.Destroy(m_device.Get());
    m_swapchain.Destroy(m_device.Get());
    m_pipeline.Destroy(m_device.Get());

    // Create
    m_pipeline.Create(m_device.Get(), m_swapchain.GetExtent(), m_renderPass.Get());
    window.GetFramebufferSize(width, height);
    m_swapchain.Create(m_physicalDevice.Get(), m_device.Get(), m_surface.Get(), width, height, m_physicalDevice.GetGraphicsQueueFamily(), m_physicalDevice.GetPresentQueueFamily());
    m_imageViews.Create(m_device.Get(), m_swapchain.GetImages(), m_swapchain.GetImageFormat());
    m_framebuffers.Create(m_device.Get(), m_renderPass.Get(), m_imageViews.Get(), m_swapchain.GetExtent());

    uint32_t count = static_cast<uint32_t>(m_framebuffers.Get().size());

    if (count == 0) {
        throw std::runtime_error("No framebuffers!");
    }

    m_commandBuffers.Create(m_device.Get(), m_commandPool.Get(), static_cast<uint32_t>(m_framebuffers.Get().size()));

}

ViewportRect VulkanRenderer::CalculateViewport(int width, int height, const ApplicationDesc& desc) {

    if (!desc.aspectRatio) {
        return {0, 0, static_cast<float>(width), static_cast<float>(height)};
    }

    float aspectRender = static_cast<float>(desc.width) / static_cast<float>(desc.height);
    float aspectScreen = static_cast<float>(width) / static_cast<float>(height);

    float w = static_cast<float>(width);
    float h = static_cast<float>(height);

    if (aspectScreen > aspectRender) {
        // ekran szerszy
        w = h * aspectRender;
    } else {
        // ekran wyższy
        h = w / aspectRender;
    }

    float x = (width  - w) * 0.5f;
    float y = (height - h) * 0.5f;

    return {x, y, w, h};

}

void VulkanRenderer::RecordCommandBuffer(uint32_t imageIndex, ApplicationDesc& desc) {

    VkCommandBuffer cmd = m_commandBuffers.Get()[imageIndex];

    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo begin{};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(cmd, &begin);

    VkClearValue clearColor = { {{0.1f, 0.1f, 0.3f, 1.0f}} };

    VkRenderPassBeginInfo rp{};
    rp.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp.renderPass = m_renderPass.Get();
    rp.framebuffer = m_framebuffers.Get()[imageIndex];
    rp.renderArea.offset = {0, 0};
    rp.renderArea.extent = m_swapchain.GetExtent();
    rp.clearValueCount = 1;
    rp.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmd, &rp, VK_SUBPASS_CONTENTS_INLINE);

    // Pipeline
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());

    // Aspect ratio
    if (desc.aspectRatio && desc.fullscreen) {

        float aspectRender = static_cast<float>(desc.width) / static_cast<float>(desc.height);
        float aspectScreen = static_cast<float>(m_swapchain.GetExtent().width) / static_cast<float>(m_swapchain.GetExtent().height);

        float width, height;

        if (aspectScreen > aspectRender) {
            // ekran szerszy → pasy po bokach
            height = static_cast<float>(m_swapchain.GetExtent().height);
            width = height * aspectRender;
        } else {
            // ekran wyższy → pasy góra/dół
            width = static_cast<float>(m_swapchain.GetExtent().width);
            height = width / aspectRender;
        }

        float x = (static_cast<float>(m_swapchain.GetExtent().width) - width) / 2.0f;
        float y = (static_cast<float>(m_swapchain.GetExtent().height) - height) / 2.0f;

        VkViewport viewport{};
        viewport.x = x;
        viewport.y = y;
        viewport.width = width;
        viewport.height = height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(cmd, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {static_cast<int>(x), static_cast<int>(y)};
        scissor.extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
        vkCmdSetScissor(cmd, 0, 1, &scissor);

        std::cout << "Viewport1: "
          << viewport.x << " " << viewport.y << " "
          << viewport.width << " " << viewport.height << std::endl;

    } else {

        VkViewport viewport{};
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = static_cast<float>(m_swapchain.GetExtent().width);
        viewport.height = static_cast<float>(m_swapchain.GetExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(cmd, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = {m_swapchain.GetExtent().width, m_swapchain.GetExtent().height};
        vkCmdSetScissor(cmd, 0, 1, &scissor);

        std::cout << "Viewport2: "
          << viewport.x << " " << viewport.y << " "
          << viewport.width << " " << viewport.height << std::endl;

    }

    vkCmdDraw(cmd, 3, 1, 0, 0);

    // Draw


    vkCmdEndRenderPass(cmd);

    vkEndCommandBuffer(cmd);

}


void VulkanRenderer::DrawFrame(Window& window, ApplicationDesc& desc) {

    vkWaitForFences(m_device.Get(), 1, &m_fences[m_currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(m_device.Get(), 1, &m_fences[m_currentFrame]);

    uint32_t imageIndex;

    VkResult result = vkAcquireNextImageKHR(
        m_device.Get(),
        m_swapchain.Get(),
        UINT64_MAX,
        m_imageAvailableSemaphores[m_currentFrame],
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        RecreateSwapchain(window, desc);
        return;
    }

    // Nagraj komendy dla tego obrazu
    RecordCommandBuffer(imageIndex, desc);

    VkSubmitInfo submit{};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = waitSemaphores;
    submit.pWaitDstStageMask = waitStages;

    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &m_commandBuffers.Get()[imageIndex];

    VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = signalSemaphores;

    VkResult submitResult = vkQueueSubmit(m_graphicsQueue, 1, &submit, m_fences[m_currentFrame]);
    if (submitResult == VK_ERROR_DEVICE_LOST) {
        throw std::runtime_error("Device lost during vkQueueSubmit");
    }
    if (submitResult != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer");
    }


    VkPresentInfoKHR present{};
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.waitSemaphoreCount = 1;
    present.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapchains[] = { m_swapchain.Get() };
    present.swapchainCount = 1;
    present.pSwapchains = swapchains;
    present.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(m_presentQueue, &present);

    if (result == VK_ERROR_DEVICE_LOST) {
        throw std::runtime_error("Device lost during present");
    }

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        RecreateSwapchain(window, desc);
    }

    m_currentFrame = (m_currentFrame + 1) % desc.maxFramesInFlight;

}

void VulkanRenderer::Shutdown(ApplicationDesc& desc) {

    vkDeviceWaitIdle(m_device.Get());

    m_pipeline.Destroy(m_device.Get());
    for (int i = 0; i < desc.maxFramesInFlight; i++) {
        vkDestroySemaphore(m_device.Get(), m_imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(m_device.Get(), m_renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(m_device.Get(), m_fences[i], nullptr);
    }
    m_commandBuffers.Destroy(m_device.Get(), m_commandPool.Get());
    m_commandPool.Destroy(m_device.Get());
    m_framebuffers.Destroy(m_device.Get());
    m_renderPass.Destroy(m_device.Get());
    m_imageViews.Destroy(m_device.Get());
    m_swapchain.Destroy(m_device.Get());
    m_device.Destroy();
    m_surface.Destroy(m_instance.Get());
    m_instance.Destroy();

}
