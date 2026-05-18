#include "pch.h"
#include "Graphics.h"

void Graphics::Initialize(Display& display, Window& window, ApplicationDesc& desc) {
    m_renderer.Initialize(display, window, desc);
}

void Graphics::Shutdown() {
    m_renderer.Shutdown();
}

void Graphics::Render(VkDevice device, ApplicationDesc& desc, float deltaTime) {

    m_renderer.Update(deltaTime, desc.HDR);
    m_renderer.Render(device, desc);

}

VulkanRenderer& Graphics::GetRenderer() {
    return m_renderer;
}
