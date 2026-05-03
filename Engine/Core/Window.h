#pragma once

#include "ApplicationDesc.h"
#include <SDL3/SDL.h>

class Window {
public:
    Window();
    ~Window();

    void Create(const ApplicationDesc& desc);
    void Destroy();

    bool ShouldClose() const;
    void SetShouldClose(bool value);

    void GetFramebufferSize(int& width, int& height) const;

    void SetWindowed(ApplicationDesc& desc);
    void SetFullscreen(ApplicationDesc& desc, Uint32 displayID);

    void SetSize(ApplicationDesc& desc);

    // Vulkan
    SDL_Window* GetHandle() const;

private:
    SDL_Window* m_window = nullptr;
    bool m_shouldClose = false;

};
