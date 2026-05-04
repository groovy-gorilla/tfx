#include "VulkanRenderer.h"

#include <array>
#include <cassert>
#include <stdexcept>
#include "../../Engine/Core/Window.h"
#include "../../Engine/Core/Error/ErrorDialog.h"
#include "VulkanUtils.h"

void VulkanRenderer::Initialize(Window& window, ApplicationDesc& desc) {

    m_instance.Create();
    m_surface.Create(m_instance.Get(), window.GetHandle());
    m_physicalDevice.Pick(m_instance.Get(), m_surface.Get());
    m_device.Create(m_physicalDevice.Get(), m_physicalDevice.GetGraphicsQueueFamily(), m_physicalDevice.GetPresentQueueFamily());

    int width = 0, height = 0;
    window.GetFramebufferSize(width, height);
    VkExtent2D extent;
    extent.width = static_cast<uint32_t>(width);
    extent.height = static_cast<uint32_t>(height);

    m_swapchain.Create(m_physicalDevice.Get(), m_device.Get(), m_surface.Get(), extent, m_physicalDevice.GetGraphicsQueueFamily(), m_physicalDevice.GetPresentQueueFamily());
    m_imageViews.Create(m_device.Get(), m_swapchain.GetImages(), m_swapchain.GetImageFormat());

    // RENDER PASS
    m_offscreenRenderPass.Create(m_device.Get(), m_swapchain.GetImageFormat(), FindDepthFormat(m_physicalDevice.Get()));
    m_swapchainRenderPass.Create(m_device.Get(), m_swapchain.GetImageFormat());

    // RESOURCES
    m_offscreenResources.Create(m_device.Get(), m_physicalDevice.Get(), extent, m_swapchain.GetImageFormat(), FindDepthFormat(m_physicalDevice.Get()), m_offscreenRenderPass.Get());

    // SAMPLER
    m_sampler.Create(m_device.Get());

    // PIPELINES
    m_offscreenPipeline.Create(m_device.Get(), m_swapchain.GetExtent(), m_offscreenRenderPass.Get());
    m_postPipeline.Create(m_device.Get(), m_swapchain.GetExtent(), m_swapchainRenderPass.Get());

    // DESCRIPTORY
    CreateDescriptorPool();
    CreateDescriptorSet();
    UpdateDescriptorSet();

    // FRAMEBUFFERS
    m_framebuffers.Create(m_device.Get(), m_swapchainRenderPass.Get(), m_imageViews.Get(), m_swapchain.GetExtent());


    m_commandPool.Create(m_device.Get(), m_physicalDevice.GetGraphicsQueueFamily());
    m_commandBuffers.Create(m_device.Get(), m_commandPool.Get(), static_cast<uint32_t>(m_framebuffers.Get().size()));

    CreateSyncObjects(desc);

    m_graphicsQueue = m_device.GetGraphicsQueue();
    m_presentQueue  = m_device.GetPresentQueue();

}

void VulkanRenderer::RecreateSwapchain(Window& window) {

    int width = 0, height = 0;
    window.GetFramebufferSize(width, height);
    while (width == 0 || height == 0) {
        window.GetFramebufferSize(width, height);
        SDL_WaitEvent(nullptr);
    }

    vkDeviceWaitIdle(m_device.Get());

    VkExtent2D extent;
    extent.width = static_cast<uint32_t>(width);
    extent.height = static_cast<uint32_t>(height);

    // Destroy
    m_commandBuffers.Destroy(m_device.Get(), m_commandPool.Get());
    m_framebuffers.Destroy(m_device.Get());
    m_postPipeline.Destroy(m_device.Get());
    m_imageViews.Destroy(m_device.Get());
    m_swapchain.Destroy(m_device.Get());
    vkFreeDescriptorSets(m_device.Get(), m_descriptorPool, 1, &m_descriptorSet);

    VkDescriptorSetLayout layout = m_postPipeline.GetDescriptorSetLayout();
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;
    vkAllocateDescriptorSets(m_device.Get(), &allocInfo, &m_descriptorSet);
    UpdateDescriptorSet();

    // Create
    m_swapchain.Create(m_physicalDevice.Get(), m_device.Get(), m_surface.Get(), extent, m_physicalDevice.GetGraphicsQueueFamily(), m_physicalDevice.GetPresentQueueFamily());
    m_imageViews.Create(m_device.Get(), m_swapchain.GetImages(), m_swapchain.GetImageFormat());
    m_postPipeline.Create(m_device.Get(), m_swapchain.GetExtent(), m_swapchainRenderPass.Get());
    m_framebuffers.Create(m_device.Get(), m_swapchainRenderPass.Get(), m_imageViews.Get(), m_swapchain.GetExtent());

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

    VkCommandBuffer commandBuffer = m_commandBuffers.Get()[imageIndex];

    vkResetCommandBuffer(commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));


    // =========================================================
    // PASS 1 — OFFSCREEN (SCENA)
    // =========================================================

    VkExtent2D extent = m_offscreenResources.GetExtent();

    std::array<VkClearValue, 2> clearValues{};

    // COLOR
    clearValues[0].color = { {0.1f, 0.1f, 0.1f, 1.0f} };

    // DEPTH
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_offscreenRenderPass.Get();
    renderPassInfo.framebuffer = m_offscreenResources.GetFramebuffer();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());;
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);                                   // BEGIN OFFSCREEN RENDER PASS
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_offscreenPipeline.Get());

    // Aspect ratio
    if (desc.aspectRatio && desc.fullscreen) {

        float aspectRender = static_cast<float>(desc.width) / static_cast<float>(desc.height);
        float aspectScreen = static_cast<float>(extent.width) / static_cast<float>(extent.height);

        float width, height;

        if (aspectScreen > aspectRender) {
            // ekran szerszy → pasy po bokach
            height = static_cast<float>(extent.height);
            width = height * aspectRender;
        } else {
            // ekran wyższy → pasy góra/dół
            width = static_cast<float>(extent.width);
            height = width / aspectRender;
        }

        float x = (static_cast<float>(extent.width) - width) / 2.0f;
        float y = (static_cast<float>(extent.height) - height) / 2.0f;

        VkViewport viewport{};
        viewport.x = x;
        viewport.y = y;
        viewport.width = width;
        viewport.height = height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {static_cast<int>(x), static_cast<int>(y)};
        scissor.extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    } else {

        VkViewport viewport{};
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = static_cast<float>(extent.width);
        viewport.height = static_cast<float>(extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = extent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    }

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);                           // RENDER SCENE

    vkCmdEndRenderPass(commandBuffer);                                                                                     // END OFFSCREEN RENDER PASS


    // TUTAJ BARRIER                                                                                                       // BARRIER !!!
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

    barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = m_offscreenResources.GetColorImage();

    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    // 👇 KLUCZOWE
    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // src
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,         // dst
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );


    // =========================================================
    // PASS 2 — SWAPCHAIN (ekran)
    // =========================================================

    VkClearValue clearValue{};
    clearValue.color = { {0.0f, 0.0f, 0.0f, 1.0f} };

    VkRenderPassBeginInfo postPass{};
    postPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    postPass.renderPass = m_swapchainRenderPass.Get();
    postPass.framebuffer =  m_framebuffers.Get()[imageIndex];
    postPass.renderArea.offset = {0, 0};
    postPass.renderArea.extent = m_swapchain.GetExtent();
    postPass.clearValueCount = 1;
    postPass.pClearValues = &clearValue;

    vkCmdBeginRenderPass(commandBuffer, &postPass, VK_SUBPASS_CONTENTS_INLINE);                                             // BEGIN SWAPCHAIN RENDER PASS

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_postPipeline.Get());                      // BIND POST PIPELINE

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,m_postPipeline.GetLayout(), 0,1, &m_descriptorSet, 0, nullptr);      // BIND DESCRIPTOR
    assert(m_descriptorSet != VK_NULL_HANDLE);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);                           // DRAW FULLSCREEN TRIANGLE

    vkCmdEndRenderPass(commandBuffer);                                                                                     // END RENDER PASS

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

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapchain(window);
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
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
        RecreateSwapchain(window);
    }

    m_currentFrame = (m_currentFrame + 1) % desc.maxFramesInFlight;

}

void VulkanRenderer::CreateSyncObjects(ApplicationDesc& desc) {

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

}

void VulkanRenderer::Shutdown(ApplicationDesc& desc) {

    vkDeviceWaitIdle(m_device.Get());

    m_offscreenPipeline.Destroy(m_device.Get());
    m_postPipeline.Destroy(m_device.Get());
    for (int i = 0; i < desc.maxFramesInFlight; i++) {
        vkDestroySemaphore(m_device.Get(), m_imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(m_device.Get(), m_renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(m_device.Get(), m_fences[i], nullptr);
    }
    m_commandBuffers.Destroy(m_device.Get(), m_commandPool.Get());
    m_commandPool.Destroy(m_device.Get());
    m_framebuffers.Destroy(m_device.Get());
    m_offscreenResources.Destroy(m_device.Get());
    m_offscreenRenderPass.Destroy(m_device.Get());
    m_imageViews.Destroy(m_device.Get());
    m_swapchain.Destroy(m_device.Get());
    m_device.Destroy();
    m_surface.Destroy(m_instance.Get());
    m_instance.Destroy();

}

void VulkanRenderer::CreateDescriptorPool() {

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;

    VK_CHECK(vkCreateDescriptorPool(m_device.Get(), &poolInfo, nullptr, &m_descriptorPool));
}

void VulkanRenderer::CreateDescriptorSet() {

    VkDescriptorSetLayout layout = m_postPipeline.GetDescriptorSetLayout();

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    VK_CHECK(vkAllocateDescriptorSets(m_device.Get(), &allocInfo, &m_descriptorSet));
}

void VulkanRenderer::UpdateDescriptorSet() {

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = m_offscreenResources.GetColorImageView();
    imageInfo.sampler = m_sampler.Get();

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = m_descriptorSet;
    write.dstBinding = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(m_device.Get(), 1, &write, 0, nullptr);
}


