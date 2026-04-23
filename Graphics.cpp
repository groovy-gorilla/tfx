#include "Graphics.h"

#include <iostream>

#include "Texture.h"
#include "Bitmap.h"
#include "BitmapRenderer.h"

Graphics::Graphics() {

    m_vulkan = nullptr;
    m_bitmap = nullptr;
    m_bitmapRenderer = nullptr;

}

Graphics::~Graphics() = default;

void Graphics::Initialize(GLFWwindow *window) {

    m_vulkan = new Vulkan;
    m_vulkan->Initialize(window);

    m_bitmap = new Bitmap;
    m_bitmap->Initialize(m_vulkan->GetDevice(),
                      m_vulkan->GetPhysicalDevice(),
                      m_vulkan->GetCommandPool(),
                      m_vulkan->GetGraphicsQueue(),
                      "gruvbox.ktx");

    m_bitmapRenderer = new BitmapRenderer;
    m_bitmapRenderer->Initialize(m_vulkan->GetDevice(), m_vulkan->GetPhysicalDevice(), m_vulkan->GetSceneRenderPass(), m_vulkan->GetExtent());
    m_bitmapRenderer->SetBitmap(*m_bitmap);

    // dlaczego offscreen a wcześniej renderpass - renderpass wywołuje błędy ale koniec konców pokazuje bitmapę
    // po włączeniu MSAA wysypuje się

}

void Graphics::Shutdown() {

    if (m_bitmapRenderer) {
        m_bitmapRenderer->Shutdown();
        delete m_bitmapRenderer;
        m_bitmapRenderer = nullptr;
    }

    if (m_bitmap) {
        m_bitmap->Shutdown(m_vulkan->GetDevice());
        delete m_bitmap;
        m_bitmap = nullptr;
    }

    if (m_vulkan) {
        m_vulkan->Shutdown();
        delete m_vulkan;
        m_vulkan = nullptr;
    }

}

void Graphics::Draw(GLFWwindow* window) {

    if (m_vulkan->IsPipelineDirty()) {
        m_bitmapRenderer->RecreatePipeline(m_vulkan->GetSceneRenderPass(), m_vulkan->GetExtent());
        m_vulkan->ClearPipelineDirty();
    }

    VkCommandBuffer cmd =  m_vulkan->BeginScene(window);

    m_bitmapRenderer->SetPosition(0, 0, SETTINGS.WIDTH, SETTINGS.HEIGHT);
    m_bitmapRenderer->Draw(cmd);

    m_vulkan->EndScene(window, cmd);



}
