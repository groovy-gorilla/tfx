#include "Window.h"

#include <iostream>
#include <stdexcept>

#include "Display.h"

Window::Window() = default;
Window::~Window() = default;

void Window::Create(const ApplicationDesc &desc) {

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
    //SDL_GetWindowSizeInPixels(m_window, &width, &height);    // nie jestem pewien czy to jest dobre
    SDL_GetWindowSize(m_window, &width, &height);
}

void Window::SetWindowed(ApplicationDesc& desc) {

    SDL_SetWindowSize(m_window, desc.width / desc.scaling, desc.height / desc.scaling);
    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    SDL_SetWindowFullscreen(m_window, false);

    desc.fullscreen = false;

}

void Window::SetFullscreen(ApplicationDesc& desc, Uint32 displayID) {

    SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(displayID, nullptr);

    modes[0]->w = desc.width;
    modes[0]->h = desc.height;

    SDL_SetWindowFullscreenMode(m_window, modes[0]);
    SDL_SetWindowFullscreen(m_window, true);

    desc.fullscreen = true;

}

void Window::SetSize(ApplicationDesc& desc) {
    SDL_SetWindowSize(m_window, desc.width / desc.scaling, desc.height / desc.scaling);
    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

SDL_Window* Window::GetHandle() const {
    return m_window;
}
