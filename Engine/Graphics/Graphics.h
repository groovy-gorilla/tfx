#pragma once

#include "Vulkan/VulkanRenderer.h"

class Display;
class Window;
struct ApplicationDesc;

class Graphics {
public:
    void Initialize(Display& display, Window& window, ApplicationDesc& desc);
    void Shutdown(ApplicationDesc& desc);
    void Render(ApplicationDesc& desc);

    VulkanRenderer& GetRenderer();

private:
    VulkanRenderer m_renderer;

};

