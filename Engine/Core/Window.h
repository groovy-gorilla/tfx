#pragma once

#include "ApplicationDesc.h"
#include "Display.h"

class Window {
public:

    void Create(ApplicationDesc& desc, Display& display);
    void Destroy();

    [[nodiscard]] bool ShouldClose() const;
    void SetShouldClose(bool value);

    void SetWindowed(ApplicationDesc& desc, Display& display);
    void SetFullscreen(ApplicationDesc& desc, Display& display);

    void SetWindowSize(ApplicationDesc& desc, Display& display);

    // Vulkan
    [[nodiscard]] SDL_Window* GetHandle() const;

    VkExtent2D GetRenderExtent(ApplicationDesc& desc);
    VkExtent2D GetWindowExtent(Display& display, ApplicationDesc& desc);

private:
    SDL_Window* m_window = nullptr;
    bool m_shouldClose = false;

    VkExtent2D m_renderExtent = {};
    VkExtent2D m_windowExtent = {};

};
