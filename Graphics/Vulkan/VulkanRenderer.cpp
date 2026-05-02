#include "VulkanRenderer.h"
#include <stdexcept>
#include "../../Engine/Core/Window.h"

void VulkanRenderer::Initialize(Window& window) {

    m_instance.Create();

    m_surface.Create(
        m_instance.Get(),
        window.GetHandle()
    );

    m_physicalDevice.Pick(
        m_instance.Get(),
        m_surface.Get()
    );

    m_device.Create(
        m_physicalDevice.Get(),
        m_physicalDevice.GetGraphicsQueueFamily(),
        m_physicalDevice.GetPresentQueueFamily()
    );

    int width, height;
    window.GetFramebufferSize(width, height);

    m_swapchain.Create(
        m_physicalDevice.Get(),
        m_device.Get(),
        m_surface.Get(),
        width,
        height,
        m_physicalDevice.GetGraphicsQueueFamily(),
        m_physicalDevice.GetPresentQueueFamily()
    );

    m_imageViews.Create(
        m_device.Get(),
        m_swapchain.GetImages(),
        m_swapchain.GetImageFormat()
    );

    m_renderPass.Create(
        m_device.Get(),
        m_swapchain.GetImageFormat()
    );

    m_framebuffers.Create(
        m_device.Get(),
        m_renderPass.Get(),
        m_imageViews.Get(),
        m_swapchain.GetExtent()
    );

    m_commandPool.Create(
        m_device.Get(),
        m_physicalDevice.GetGraphicsQueueFamily()
    );

    m_commandBuffers.Create(
        m_device.Get(),
        m_commandPool.Get(),
        static_cast<uint32_t>(m_framebuffers.Get().size())
    );

    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_fences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semInfo{};
    semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(m_device.Get(), &semInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_device.Get(), &semInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_device.Get(), &fenceInfo, nullptr, &m_fences[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create sync objects");
            }
    }

    m_graphicsQueue = m_device.GetGraphicsQueue();
    m_presentQueue  = m_device.GetPresentQueue();

}

void VulkanRenderer::RecreateSwapchain(Window& window) {

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

    // Create
    m_swapchain.Create(
        m_physicalDevice.Get(),
        m_device.Get(),
        m_surface.Get(),
        width,
        height,
        m_physicalDevice.GetGraphicsQueueFamily(),
        m_physicalDevice.GetPresentQueueFamily()
    );

    m_imageViews.Create(
        m_device.Get(),
        m_swapchain.GetImages(),
        m_swapchain.GetImageFormat()
    );

    m_framebuffers.Create(
        m_device.Get(),
        m_renderPass.Get(),
        m_imageViews.Get(),
        m_swapchain.GetExtent()
    );

    uint32_t count = static_cast<uint32_t>(m_framebuffers.Get().size());

    if (count == 0) {
        throw std::runtime_error("No framebuffers!");
    }

    m_commandBuffers.Create(
        m_device.Get(),
        m_commandPool.Get(),
        static_cast<uint32_t>(m_framebuffers.Get().size())
    );


}

void VulkanRenderer::RecordCommandBuffer(uint32_t imageIndex) {

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

    // 🔥 tu później pipeline i draw

    vkCmdEndRenderPass(cmd);

    vkEndCommandBuffer(cmd);

}

void VulkanRenderer::DrawFrame(Window& window) {

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
        RecreateSwapchain(window);
        return;
    }

    // Nagraj komendy dla tego obrazu
    RecordCommandBuffer(imageIndex);

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
        RecreateSwapchain(window);
    }

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

}

void VulkanRenderer::Shutdown() {

    vkDeviceWaitIdle(m_device.Get());

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
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
