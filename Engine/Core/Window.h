#pragma once

#include <SDL3/SDL.h>

struct WindowDesc {
    const char* title = "Indigo Engine";
    uint32_t width = 640;
    uint32_t height = 480;
    float scaling = 1.0f;
};

class Window {
public:
    Window();
    ~Window();

    void Create(const WindowDesc& desc);
    void Destroy();

    bool ShouldClose() const;
    void SetShouldClose(bool value);

    // Vulkan
    SDL_Window* GetHandle() const;

private:
    SDL_Window* m_window = nullptr;
    bool m_shouldClose = false;
};
