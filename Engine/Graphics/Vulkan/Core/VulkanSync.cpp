#include "pch.h"
#include "VulkanSync.h"
#include "Debug/ErrorDialog.h"

void VulkanSync::Create(VkDevice device, uint32_t maxFramesInFlight) {

    m_imageAvailableSemaphores.resize(maxFramesInFlight);
    m_renderFinishedSemaphores.resize(maxFramesInFlight);
    m_renderFences.resize(maxFramesInFlight);

    // SEMAPHORES
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    // FENCES
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < maxFramesInFlight; i++) {
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]));
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]));
        VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &m_renderFences[i]));
    }

    std::cout << "[Vulkan] Sync objects created" << std::endl;

}

void VulkanSync::Destroy(VkDevice device) {

    for (VkSemaphore semaphore : m_imageAvailableSemaphores) {
        if (semaphore != VK_NULL_HANDLE) {
            vkDestroySemaphore(device, semaphore, nullptr);
        }
    }

    for (VkSemaphore semaphore : m_renderFinishedSemaphores) {
        if (semaphore != VK_NULL_HANDLE) {
            vkDestroySemaphore(device, semaphore, nullptr);
        }
    }

    for (VkFence fence : m_renderFences) {
        if (fence != VK_NULL_HANDLE) {
            vkDestroyFence(device, fence, nullptr);
        }
    }

    m_imageAvailableSemaphores.clear();
    m_renderFinishedSemaphores.clear();
    m_renderFences.clear();

    std::cout << "[Vulkan] Sync objects destroyed" << std::endl;

}

void VulkanSync::Wait(VkDevice device) {
    VK_CHECK(vkWaitForFences(device, 1, &m_renderFences[m_currentFrame], VK_TRUE, UINT64_MAX));
}

void VulkanSync::Reset(VkDevice device) {
    VK_CHECK(vkResetFences(device, 1, &m_renderFences[m_currentFrame]));
}

VkSemaphore VulkanSync::GetImageAvailableSemaphore() const {
    return m_imageAvailableSemaphores[m_currentFrame];
}

VkSemaphore VulkanSync::GetRenderFinishedSemaphore() const {
    return m_renderFinishedSemaphores[m_currentFrame];
}

VkFence VulkanSync::GetFence() const {
    return m_renderFences[m_currentFrame];
}

uint32_t VulkanSync::GetCurrentFrame() const {
    return m_currentFrame;
}

void VulkanSync::NextFrame(uint32_t maxFramesInFlight) {
    m_currentFrame = (m_currentFrame + 1) % maxFramesInFlight;
}