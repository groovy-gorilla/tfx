#include "Graphics.h"

void Graphics::Initialize(Window& window) {
    m_renderer.Initialize(window);
}

void Graphics::Shutdown() {
    m_renderer.Shutdown();
}

void Graphics::DrawFrame(Window& window) {
    m_renderer.DrawFrame(window);
}

void Graphics::RecreateSwapchain(Window& window) {
    m_renderer.RecreateSwapchain(window);
}
