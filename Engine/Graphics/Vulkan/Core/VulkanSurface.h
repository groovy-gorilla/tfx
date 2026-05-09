#pragma once

class VulkanSurface {
public:
    void Create(VkInstance instance, SDL_Window* window);
    void Destroy(VkInstance instance);

    [[nodiscard]] VkSurfaceKHR Get() const;

private:
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;


};