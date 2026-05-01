#include "VulkanSurface.h"

#include <iostream>
#include <ostream>
#include <SDL3/SDL_vulkan.h>
#include <stdexcept>


void VulkanSurface::Create(VkInstance instance, SDL_Window* window) {

    if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, &m_surface)) {
        throw std::runtime_error(SDL_GetError());
    }

    std::cout << "[Vulkan] Surface created" << std::endl;

}

void VulkanSurface::Destroy(VkInstance instance) {

    if (m_surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance, m_surface, nullptr);
        m_surface = VK_NULL_HANDLE;
        std::cout << "[Vulkan] Surface destroyed" << std::endl;
    }

}

VkSurfaceKHR VulkanSurface::Get() const {
    return m_surface;
}

