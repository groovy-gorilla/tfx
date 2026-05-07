#pragma once

#include "ApplicationDesc.h"
#include <SDL3/SDL.h>
#include "Display.h"

class Window {
public:
    Window();
    ~Window();

    void Create(const ApplicationDesc& desc, Display display);
    void Destroy();

    bool ShouldClose() const;
    void SetShouldClose(bool value);

    void GetFramebufferSize(int& width, int& height) const;

    void SetWindowed(ApplicationDesc& desc, Display display);
    void SetFullscreen(ApplicationDesc& desc, Uint32 displayID);

    void UpdateSize(ApplicationDesc& desc, Display display);

    // Vulkan
    SDL_Window* GetHandle() const;

private:
    SDL_Window* m_window = nullptr;
    bool m_shouldClose = false;

    float m_scaling = 1.0f;

};
