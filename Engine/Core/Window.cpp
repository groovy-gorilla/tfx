#include "pch.h"
#include "Window.h"

void Window::Create(ApplicationDesc &desc, Display& display) {

    setenv("GTK_THEME", "Adwaita:dark", 1);

    GetWindowExtent(display, desc);

    m_window = SDL_CreateWindow(
        desc.TITLE,
        desc.WIDTH / display.GetScaling(),
        desc.HEIGHT / display.GetScaling(),
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

void Window::SetWindowed(ApplicationDesc& desc, Display& display) {

    SDL_SetWindowFullscreen(m_window, false);
    SDL_SetWindowSize(m_window, desc.WIDTH / display.GetScaling(), desc.HEIGHT / display.GetScaling());
    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

}

void Window::SetFullscreen(ApplicationDesc& desc, Display& display) {

    SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(display.GetPrimaryDisplay().id, nullptr);

    modes[0]->w = desc.WIDTH;
    modes[0]->h = desc.HEIGHT;

    SDL_SetWindowFullscreenMode(m_window, modes[0]);
    SDL_SetWindowFullscreen(m_window, true);

}

void Window::SetWindowSize(ApplicationDesc& desc, Display& display) {

    SDL_SetWindowSize(m_window, desc.WIDTH / display.GetScaling(), desc.HEIGHT / display.GetScaling());

}

SDL_Window* Window::GetHandle() const {
    return m_window;
}

VkExtent2D Window::GetRenderExtent(ApplicationDesc& desc) {

    m_renderExtent.width = desc.WIDTH;
    m_renderExtent.height = desc.HEIGHT;

    return m_renderExtent;

}

VkExtent2D Window::GetWindowExtent(Display& display, ApplicationDesc& desc) {

    if (desc.FULLSCREEN) {
        m_windowExtent.width = static_cast<uint32_t>(display.GetCurrentDisplayMode()->w * display.GetScaling());
        m_windowExtent.height = static_cast<uint32_t>(display.GetCurrentDisplayMode()->h * display.GetScaling());
    } else {
        m_windowExtent.width = desc.WIDTH;
        m_windowExtent.height = desc.HEIGHT;
    }

    return m_windowExtent;

}

