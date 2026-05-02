#include "Window.h"

#include <iostream>
#include <ostream>
#include <stdexcept>

Window::Window() = default;
Window::~Window() = default;

void Window::Create(const WindowDesc &desc) {

    setenv("GTK_THEME", "Adwaita:dark", 1);

    m_window = SDL_CreateWindow(
        desc.title,
        desc.width / desc.scaling,
        desc.height / desc.scaling,
        SDL_WINDOW_VULKAN
    );

    if (!m_window) {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_ShowWindow(m_window);

}

void Window::Destroy() {
    if (m_window) {
        SDL_DestroyWindow(m_window);
    }
}

bool Window::ShouldClose() const {
    return m_shouldClose;
}

void Window::SetShouldClose(bool value) {
    m_shouldClose = value;
}

void Window::GetFramebufferSize(int& width, int& height) const {
    SDL_GetWindowSizeInPixels(m_window, &width, &height);
}

void Window::SetWindowed(int width, int height, float scaling) {

    SDL_SetWindowSize(m_window, width / scaling, height / scaling);
    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    SDL_SetWindowFullscreen(m_window, false);
    m_fullscreen = false;

}

void Window::SetFullscreen(int width, int height, Uint32 displayID) {

    int count;
    SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(displayID, &count);

    const SDL_DisplayMode* selected = nullptr;

    for (int i = 0; i < count; i++) {
        if (modes[i]->w == width && modes[i]->h == height) {
            selected = modes[i];
            break;
        }
    }

    if (!selected && count > 0) {
        selected = modes[0]; // fallback
    }

    if (selected) {
        SDL_SetWindowFullscreenMode(m_window, selected);
        SDL_SetWindowFullscreen(m_window, true);
        m_fullscreen = true;
    }

}

bool Window::IsFullscreen() const {
    return m_fullscreen;
}

SDL_Window* Window::GetHandle() const {
    return m_window;
}
