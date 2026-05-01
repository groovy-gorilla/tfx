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
    m_display.SetCurrentMode(0);

    WindowDesc desc;
    desc.title = "Indigo Engine";
    desc.width = m_display.GetCurrentMode().width;
    desc.height = m_display.GetCurrentMode().height;
    desc.scaling = m_display.GetScaling();

    m_window.Create(desc);

    m_input.Initialize(SDL_SCANCODE_COUNT);
    SDLInput sdlInput(m_input);

    m_inputMapping.Bind("Quit", Key::Escape);

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
            sdlInput.ProcessEvent(event); // 🔥 KLUCZ
        }

        // QUIT
        if (m_inputMapping.IsActionPressed(m_input, "Quit")) break;

    }


    m_graphics.Shutdown();
    m_window.Destroy();
    SDL_Quit();

}
