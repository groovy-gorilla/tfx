#pragma once

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>

class VulkanSurface {
public:
    void Create(VkInstance instance, SDL_Window* window);
    void Destroy(VkInstance instance);

    VkSurfaceKHR Get() const;

private:
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;

};