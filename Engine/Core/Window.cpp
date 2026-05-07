#include "Window.h"

#include <iostream>
#include <stdexcept>

#include "Display.h"



// Okno powinno mieć możliwość UpdateScaling w przypadku, gdyby w trakcie działania aplikacji
// użytkownik zmienił skalowanie desktopu w systemie




Window::Window() = default;
Window::~Window() = default;

void Window::Create(const ApplicationDesc &desc, Display display) {

    m_scaling = display.GetScaling();

    setenv("GTK_THEME", "Adwaita:dark", 1);

    m_window = SDL_CreateWindow(
        desc.TITLE,
        desc.WIDTH / m_scaling,
        desc.HEIGHT / m_scaling,
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

void Window::SetWindowed(ApplicationDesc& desc, Display display) {

    UpdateSize(desc, display);

    SDL_SetWindowFullscreen(m_window, false);

    desc.FULLSCREEN = false;

}

void Window::SetFullscreen(ApplicationDesc& desc, Uint32 displayID) {

    SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(displayID, nullptr);

    modes[0]->w = desc.WIDTH;
    modes[0]->h = desc.HEIGHT;

    SDL_SetWindowFullscreenMode(m_window, modes[0]);
    SDL_SetWindowFullscreen(m_window, true);

    desc.FULLSCREEN = true;

}

void Window::UpdateSize(ApplicationDesc& desc, Display display) {

    m_scaling = display.GetScaling();

    SDL_SetWindowSize(m_window, desc.WIDTH / m_scaling, desc.HEIGHT / m_scaling);
    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

}


SDL_Window* Window::GetHandle() const {
    return m_window;
}
