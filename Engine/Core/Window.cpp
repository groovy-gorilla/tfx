#include "Window.h"

#include <stdexcept>

Window::Window() = default;
Window::~Window() = default;

void Window::Create(const WindowDesc &desc) {

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

SDL_Window* Window::GetHandle() const {
    return m_window;
}
