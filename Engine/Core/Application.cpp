#include "Application.h"

#include <algorithm>
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

    m_window.Create(m_desc);

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
    m_graphics.Initialize(m_window, m_desc);

    while (!m_window.ShouldClose()) {

        //if (m_framebufferResized) {
        //    m_framebufferResized = false;
        //    m_graphics.RecreateSwapchain(m_window);
        //}

        //m_graphics.DrawFrame(m_window, m_desc);

        //if (m_pendingFullscreen) {
        //    SDL_SetWindowFullscreenMode(m_window.GetHandle(), m_display.GetDisplayNativeMode());
        //    SDL_SetWindowFullscreen(m_window.GetHandle(), m_desc.fullscreen);
        //    m_pendingFullscreen = false;
        //}

        m_input.BeginFrame();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:                                   // kliknięcie X w oknie
                    //m_window.SetShouldClose(true);
                    break;
                case SDL_EVENT_QUIT:                                                     // zakończenie programu
                    m_window.SetShouldClose(true);
                    break;
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:                               // zmiana rozmiaru framebuffer
                    //m_framebufferResized = true;
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

            m_sdlInput.ProcessEvent(event);

        }

        // ASPECT RATIO
        //if (actions.IsActionPressed(m_input, "Aspect")) {
        //    m_desc.aspectRatio = !m_desc.aspectRatio;
        //}

        // WINDOWED
        //if (actions.IsActionPressed(m_input, "Windowed")) {
        //    if (m_desc.fullscreen) {
        //        m_window.SetWindowed(m_desc);
        //        m_window.SetSize(m_desc);
        //    } else {
        //        m_window.SetFullscreen(m_desc, m_display.GetPrimaryDisplay().id);
        //    }
        //}

        // QUIT
        if (actions.IsActionPressed(m_input, "Quit")) break;

        // SCREEN RESOLUTION
        //int size = m_display.GetDisplayModes().size();
        //std::vector<Mode> m = m_display.GetDisplayModes();
        //auto it = std::find_if(m.begin(), m.end(),
        //    [&](const Mode& r) {
        //        return r.width == m_desc.width && r.height == m_desc.height;
        //    });

        //static int i = -1;
        //if (it != m_display.GetDisplayModes().end()) {
        //    i = std::distance(m.begin(), it);
        //}

        //if (actions.IsActionPressed(m_input, "ResolutionDown")) {
        //    i++;
        //    if (i > (size-1)) i = (size-1);
        //    SetResolution(m[i]);
        //    std::cout << "Resolution set to: " << m[i].width << "x" << m[i].height << std::endl;
        //}

        //if (actions.IsActionPressed(m_input, "ResolutionUp")) {
        //    i--;
        //    if (i < 0) i = 0;
        //    SetResolution(m[i]);
        //    std::cout << "Resolution set to: " << m[i].width << "x" << m[i].height << std::endl;
        //}








    }


    m_graphics.Shutdown(m_desc);
    m_window.Destroy();
    SDL_Quit();

}

void Application::SetResolution(const Mode& res) {

    // 1. Window desc
    //m_desc.width = res.width;
    //m_desc.height = res.height;

    // 2. Zmienia rozmiar okna
    //m_window.SetSize(m_desc);

    // 3. Renderer ogarnia swapchain
    //m_graphics.GetRenderer().RecreateSwapchain(m_window);

}

