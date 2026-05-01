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

    VkSemaphoreCreateInfo semInfo{};
    semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    vkCreateSemaphore(m_device.Get(), &semInfo, nullptr, &m_imageAvailableSemaphore);
    vkCreateSemaphore(m_device.Get(), &semInfo, nullptr, &m_renderFinishedSemaphore);
    vkCreateFence(m_device.Get(), &fenceInfo, nullptr, &m_fence);

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

    m_framebuffers.Destroy(m_device.Get());
    m_imageViews.Destroy(m_device.Get());
    m_swapchain.Destroy(m_device.Get());

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



}

void VulkanRenderer::RecordCommandBuffer(uint32_t imageIndex) {

    VkCommandBuffer cmd = m_commandBuffers.Get()[imageIndex];

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

    vkWaitForFences(m_device.Get(), 1, &m_fence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_device.Get(), 1, &m_fence);

    uint32_t imageIndex;

    VkResult result = vkAcquireNextImageKHR(
        m_device.Get(),
        m_swapchain.Get(),
        UINT64_MAX,
        m_imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapchain(window);
        return;
    }

    // Nagraj komendy dla tego obrazu
    RecordCommandBuffer(imageIndex);

    VkSubmitInfo submit{};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = waitSemaphores;
    submit.pWaitDstStageMask = waitStages;

    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &m_commandBuffers.Get()[imageIndex];

    VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore };
    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_graphicsQueue, 1, &submit, m_fence) != VK_SUCCESS) {
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

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        RecreateSwapchain(window);
    }

}

void VulkanRenderer::Shutdown() {

    vkDeviceWaitIdle(m_device.Get());

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
