#include "System.h"
#include "Settings.h"
#include <algorithm>
#include <iostream>

System::System() {
    m_window = nullptr;
    m_displays = nullptr;
    m_currentDisplayMode = nullptr;
    m_graphics = nullptr;
    m_input = nullptr;
}

System::~System() = default;

void System::Initialize() {

    InitializeWindow();

    m_graphics = new Graphics();
    m_graphics->Initialize(m_window);

    m_input = new Input();
    m_input->Initialize();

}

void System::Shutdown() {

    m_graphics->Shutdown();
    delete m_graphics;
    m_graphics = nullptr;

    delete m_input;
    m_input = nullptr;

    ShutdownWindow();

}

void System::Run() {

    Loop();

}

void System::InitializeWindow() {

    setenv("GTK_THEME", "Adwaita:dark", 1);

    // SDL Init
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error(SDL_GetError());
    }

    // Pobiera listę monitorów
    int count = 0;
    m_displays = SDL_GetDisplays(&count);
    if (!m_displays) {
        throw std::runtime_error(SDL_GetError());
    }

    // Wypisuje listę monitorów
    for (int i = 0; i < count; i++) {
        const char* name = SDL_GetDisplayName(m_displays[i]);

        SDL_Rect bounds;
        SDL_GetDisplayBounds(m_displays[i], &bounds);

        int c = 0;
        SDL_DisplayMode** m = SDL_GetFullscreenDisplayModes(m_displays[i], &c);

        std::cout << "MONITOR [" << i << "]" << std::endl;
        std::cout << "   NAME: " << name << std::endl;
        std::cout << "   POSITION: " << bounds.x << ", " << bounds.y << std::endl;
        std::cout << "   MODE: " << m[0]->w << "x" << m[0]->h << std::endl;
    }

    // Wybiera monitor
    SDL_DisplayID currentDisplay = m_displays[0];   //SDL_GetPrimaryDisplay() - nie działa dobrze

    // Pobiera rozdzielczość aktualnie wyświetlaną przez Linux na wybranym monitorze
    SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(currentDisplay, nullptr);
    m_currentDisplayMode = modes[0];

    // Wyliczanie skalowania
    const SDL_DisplayMode* mcurr = SDL_GetCurrentDisplayMode(currentDisplay);
    m_scaling = static_cast<float>(m_currentDisplayMode->w) / mcurr->w;
    std::cout << "SCALING: " << m_scaling << std::endl;

    // Tworzy zwykłe okno (dopiero później w trakcie działania aplikacji przełącza je na FULLSCREEN jeśli tak ustawiono)
    m_window = SDL_CreateWindow(SETTINGS.TITLE.c_str(), SETTINGS.WIDTH / m_scaling, SETTINGS.HEIGHT / m_scaling, SDL_WINDOW_VULKAN);

}

void System::ShutdownWindow() {

    SDL_free(m_currentDisplayMode);
    m_currentDisplayMode = nullptr;

    SDL_free(m_displays);
    m_displays = nullptr;

    SDL_DestroyWindow(m_window);
    SDL_Quit();

}

void System::Loop() {

    bool running = true;
    SDL_Event event;
    static bool done = false;

    while (running) {

        // RYSOWANIE
        m_graphics->Draw(m_window);

        // OBSŁUGA KLAWIATURY
        m_input->BeginFrame();

        // OBSŁUGA ZDARZEŃ OKNA
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:                                   // kliknięcie X w oknie
                    running = false;
                    break;
                case SDL_EVENT_QUIT:                                                     // zakończenie programu
                    running = false;
                    break;
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:                               // zmiana rozmiaru framebuffer
                    m_graphics->m_vulkan->SetFramebufferResized(true);
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
        }

        // SCREEN RESOLUTION
        int size = m_graphics->m_vulkan->GetVideoModes().size();
        std::vector<Vulkan::Resolution> m = m_graphics->m_vulkan->GetVideoModes();
        auto it = std::find_if(m.begin(), m.end(),
            [&](const Vulkan::Resolution& r) {
                return r.w == SETTINGS.WIDTH && r.h == SETTINGS.HEIGHT;
            });
        static int i = -1;
        if (it != m_graphics->m_vulkan->GetVideoModes().end()) {
            i = std::distance(m.begin(), it);
        }
        if (m_input->IsKeyPressed(SDL_SCANCODE_EQUALS)) {
            i++;
            if (i > (size-1)) i = (size-1);
            m_graphics->m_vulkan->SetResolution(m_window, m_graphics->m_vulkan->GetVideoModes().at(i).w, m_graphics->m_vulkan->GetVideoModes().at(i).h, m_scaling);
            std::cout << "Resolution set to: " << SETTINGS.WIDTH << "x" << SETTINGS.HEIGHT << std::endl;
        }

        if (m_input->IsKeyPressed(SDL_SCANCODE_MINUS)){
            i--;
            if (i < 0) i = 0;
            m_graphics->m_vulkan->SetResolution(m_window, m_graphics->m_vulkan->GetVideoModes().at(i).w, m_graphics->m_vulkan->GetVideoModes().at(i).h, m_scaling);
            std::cout << "Resolution set to: " << SETTINGS.WIDTH << "x" << SETTINGS.HEIGHT << std::endl;
        }

        // MSAA
        if (m_input->IsKeyPressed(SDL_SCANCODE_M)) {
            if (SETTINGS.MSAA_SAMPLES == VK_SAMPLE_COUNT_1_BIT) {
                m_graphics->m_vulkan->SetMSAA(VK_SAMPLE_COUNT_16_BIT);
            } else {
                m_graphics->m_vulkan->SetMSAA(VK_SAMPLE_COUNT_1_BIT);
            }
        }

        // FILTER
        if (m_input->IsKeyPressed(SDL_SCANCODE_F)) {
            if (SETTINGS.FILTER == VK_FILTER_NEAREST) {
                m_graphics->m_vulkan->SetFilter(VK_FILTER_LINEAR);
            } else {
                m_graphics->m_vulkan->SetFilter(VK_FILTER_NEAREST);
            }
        }

        // ASPECT RATIO
        if (m_input->IsKeyPressed(SDL_SCANCODE_A)) {
            if (SETTINGS.KEEP_ASPECT_RATIO) {
                m_graphics->m_vulkan->SetAspectRatioEnabled(false);
            } else {
                m_graphics->m_vulkan->SetAspectRatioEnabled(true);
            }
        }

        // FULLSCREEN/WINDOWED
        if (m_input->IsKeyPressed(SDL_SCANCODE_W)) {
            if (SETTINGS.FULLSCREEN) {
                // WINDOWED
                m_graphics->m_vulkan->SetFullscreenEnabled(m_window, false, m_currentDisplayMode, m_scaling);
            } else {
                // FULLSCREEN
                m_graphics->m_vulkan->SetFullscreenEnabled(m_window, true, m_currentDisplayMode, m_scaling);
            }
        }

        // QUIT
        if (m_input->IsKeyPressed(SDL_SCANCODE_ESCAPE)) break;


        m_input->EndFrame();

        if (!done) {
            SDL_Delay(500);
            SDL_SetWindowFullscreenMode(m_window, m_currentDisplayMode);
            SDL_SetWindowFullscreen(m_window, true);
            m_graphics->m_vulkan->SetFullscreenEnabled(m_window, SETTINGS.FULLSCREEN, m_currentDisplayMode, m_scaling);
            done = true;
        }

    }

    vkDeviceWaitIdle(m_graphics->m_vulkan->GetDevice());

}



