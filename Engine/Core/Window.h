#pragma once

#include <SDL3/SDL.h>

struct WindowDesc {
    const char* title;
    uint32_t width;
    uint32_t height;
    float scaling;
};

class Window {
public:
    Window();
    ~Window();

    void Create(const WindowDesc& desc);
    void Destroy();

    bool ShouldClose() const;
    void SetShouldClose(bool value);

    void GetFramebufferSize(int& width, int& height) const;

    void SetWindowed(int width, int height, float scaling);
    void SetFullscreen(int width, int height, Uint32 displayID);

    bool IsFullscreen() const;

    // Vulkan
    SDL_Window* GetHandle() const;

private:
    SDL_Window* m_window = nullptr;
    bool m_shouldClose = false;
    bool m_fullscreen = false;

};
