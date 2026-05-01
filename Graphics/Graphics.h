#pragma once

#include "Vulkan/VulkanRenderer.h"

class Window;

class Graphics {
public:
    void Initialize(Window& window);
    void Shutdown();

    void DrawFrame(Window& window);

    void RecreateSwapchain(Window& window);

private:
    VulkanRenderer m_renderer;

};

