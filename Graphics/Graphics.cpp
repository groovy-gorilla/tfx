#include "Graphics.h"

void Graphics::Initialize(Window& window, ApplicationDesc& desc) {
    m_renderer.Initialize(window, desc);
}

void Graphics::Shutdown(ApplicationDesc& desc) {
    m_renderer.Shutdown(desc);
}

void Graphics::DrawFrame(Window& window, ApplicationDesc& desc) {
    m_renderer.DrawFrame(window, desc);
}

void Graphics::RecreateSwapchain(Window& window, ApplicationDesc& desc) {
    m_renderer.RecreateSwapchain(window, desc);
}

VulkanRenderer& Graphics::GetRenderer() {
    return m_renderer;
}
