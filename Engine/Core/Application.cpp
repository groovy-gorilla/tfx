#include "Application.h"

#include <iostream>
#include <ostream>

#include "Window.h"
#include <stdexcept>
#include <SDL3/SDL.h>
#include "../../Engine/Platform/SDL/SDLInput.h"

void Application::Run() {

    setenv("GTK_THEME", "Adwaita:dark", true);

    // SDL Init
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error(SDL_GetError());
    }

    m_display.Initialize();
    m_display.SetCurrentDisplay(0);
    m_display.SetCurrentMode(21); // 21 to 800x600

    WindowDesc desc;
    desc.title = "Indigo Engine";
    desc.width = m_display.GetCurrentMode()->w;
    desc.height = m_display.GetCurrentMode()->h;
    desc.scaling = m_display.GetScaling();

    m_window.Create(desc);

    m_input.Initialize(SDL_SCANCODE_COUNT);

    m_sdlInput.Initialize(&m_input);

    actions.Bind("Quit", Key::Escape);
    actions.Bind("Windowed", Key::W);
    actions.Bind("Aspect", Key::A);
    actions.Bind("Filter", Key::F);
    actions.Bind("MSAA", Key::M);
    actions.Bind("ResolutionUp", Key::Equals);
    actions.Bind("ResolutionDown", Key::Minus);
    actions.Bind("Monitor1", Key::Num1);
    actions.Bind("Monitor2", Key::Num2);

    // VULKAN
    m_graphics.Initialize(m_window);


    while (!m_window.ShouldClose()) {

        m_graphics.DrawFrame(m_window);

        m_input.BeginFrame();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:                                   // kliknięcie X w oknie
                    m_window.SetShouldClose(true);
                    break;
                case SDL_EVENT_QUIT:                                                     // zakończenie programu
                    m_window.SetShouldClose(true);
                    break;
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:                               // zmiana rozmiaru framebuffer
                    m_graphics.RecreateSwapchain(m_window);
                    break;
                case SDL_EVENT_WINDOW_RESIZED:                                          // zmiana rozmiaru okna
                    // ...
                    break;
                case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:                            // zmiana skali w systemie
                    // ...
                    break;
                case SDL_EVENT_WINDOW_MINIMIZED:                                        // minimalizacja okna (utrata fokusa i okno ma wtedy zwykle rozmiar 0x0
                    // minimized = true;                                                // if (paused) SDL_Delay(10); continue; --> nie renderuję do GPU
                    break;
                case SDL_EVENT_WINDOW_RESTORED:                                         // przywrócenie okna z mimimalizacji
                    // minimized = false;
                    // recreate swapchain
                    break;
                case SDL_EVENT_WINDOW_MAXIMIZED:                                        // maksymalizacja okna
                    // ...
                    break;
                case SDL_EVENT_WINDOW_FOCUS_LOST:                                       // utrata fokusa --> robimy pauzę
                    // focused = false;                                                 // if (paused) SDL_Delay(10); continue;
                    // SDL_SetRelativeMouseMode(SDL_FALSE);                             // oddanie kursora myszy --> przywrócnie np. po kliknięciu w okno
                    // bool paused = minimized || !focused;
                    break;
                case SDL_EVENT_WINDOW_FOCUS_GAINED:                                     // przywrócenie fokusa
                    // focused = true;                                                  // ale co jeśli podczas utraty fokusa doszło np. do zmiany skali?
                    // SDL_SetRelativeMouseMode(SDL_TRUE);
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:                                       // uchwycenie myszy po kliknięciu nią w okno
                    // SDL_SetRelativeMouseMode(SDL_TRUE);
                    break;
                case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
                    // ...
                    break;
                case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
                    // ...
                    break;
                default:
                    // ...
                    break;
            }

            m_sdlInput.ProcessEvent(event); // KLUCZ

        }

        // WINDOWED
        if (actions.IsActionPressed(m_input, "Windowed")) {
            if (m_window.IsFullscreen()) {
                m_window.SetWindowed(desc.width, desc.height, desc.scaling);
            } else {
                m_window.SetFullscreen(desc.width, desc.height, m_display.GetCurrentDisplay().id);
            }
        }

        // QUIT
        if (actions.IsActionPressed(m_input, "Quit")) break;

        // Zmiana położenia okna fullscreen pomiędzy monitorami
        if (m_window.IsFullscreen()) {

            // MONITOR 1
            if (actions.IsActionPressed(m_input, "Monitor1")) {
                m_display.SetCurrentDisplay(0);
                m_display.SetCurrentMode(21);
                SDL_SetWindowSize(m_window.GetHandle(), m_display.GetCurrentMode()->w / m_display.GetScaling(), m_display.GetCurrentMode()->h / m_display.GetScaling());
                SDL_Rect bounds;
                SDL_GetDisplayBounds(0, &bounds);
                SDL_SetWindowPosition(m_window.GetHandle(), bounds.x + bounds.w / 2, bounds.y + bounds.h / 2);
                SDL_SetWindowFullscreenMode(m_window.GetHandle(), m_display.GetCurrentMode());
                std::cout << "MONITOR 0" << std::endl;
            }

            // MONITOR 2
            if (actions.IsActionPressed(m_input, "Monitor2")) {
                m_display.SetCurrentDisplay(1);
                m_display.SetCurrentMode(14);
                SDL_SetWindowSize(m_window.GetHandle(), m_display.GetCurrentMode()->w / m_display.GetScaling(), m_display.GetCurrentMode()->h / m_display.GetScaling());
                SDL_Rect bounds;
                SDL_GetDisplayBounds(1, &bounds);
                SDL_SetWindowPosition(m_window.GetHandle(), bounds.x + bounds.w / 2, bounds.y + bounds.h / 2);
                SDL_SetWindowFullscreenMode(m_window.GetHandle(), m_display.GetCurrentMode());
                std::cout << "MONITOR 1" << std::endl;
            }

        }




        static bool done = false;
        if (!done) {
            //SDL_Delay(10);
            SDL_SetWindowFullscreenMode(m_window.GetHandle(), m_display.GetCurrentMode());
            SDL_SetWindowFullscreen(m_window.GetHandle(), true);
            m_window.SetFullscreen(desc.width, desc.height, m_display.GetCurrentDisplay().id);
            //m_window.SetWindowed(desc.width, desc.height, desc.scaling);
            done = true;
        }

    }


    m_graphics.Shutdown();
    m_window.Destroy();
    SDL_Quit();

}

