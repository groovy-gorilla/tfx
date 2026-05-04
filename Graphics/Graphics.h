#pragma once

#include "Vulkan/VulkanRenderer.h"

class Window;

class Graphics {
public:
    void Initialize(Window& window, ApplicationDesc& desc);
    void Shutdown(ApplicationDesc& desc);

    void DrawFrame(Window& window, ApplicationDesc& desc);

    void RecreateSwapchain(Window& window);

    VulkanRenderer& GetRenderer();

private:
    VulkanRenderer m_renderer;

};

