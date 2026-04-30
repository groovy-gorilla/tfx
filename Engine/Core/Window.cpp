#include "Window.h"

Window::Window() = default;
Window::~Window() = default;

void Window::Create(const WindowDesc &desc) {

    m_window = SDL_CreateWindow(
        desc.title,
        desc.width / desc.scaling,
        desc.height / desc.scaling,
        SDL_WINDOW_VULKAN
    );

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

SDL_Window* Window::GetHandle() const {
    return m_window;
}
