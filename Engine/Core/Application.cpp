#include "pch.h"
#include "Application.h"

void Application::Run() {

    setenv("GTK_THEME", "Adwaita:dark", true);

    // SDL Init
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error(SDL_GetError());
    }

    m_display.Initialize();

    m_window.Create(m_desc, m_display);

    m_input.Initialize(SDL_SCANCODE_COUNT);

    m_sdlInput.Initialize(&m_input);

    actions.Bind("Quit", Key::Escape);
    actions.Bind("Windowed", Key::W);
    actions.Bind("Aspect", Key::A);
    actions.Bind("Filter", Key::F);
    actions.Bind("AA", Key::M);
    actions.Bind("VSync", Key::V);
    actions.Bind("Screenshot", Key::S);
    actions.Bind("ResolutionUp", Key::Equals);
    actions.Bind("ResolutionDown", Key::Minus);

    // VULKAN
    m_graphics.Initialize(m_display, m_window, m_desc);

    auto lastTime = std::chrono::high_resolution_clock::now();
    float timer = 0.0f;
    uint32_t frames = 0;

    while (!m_window.ShouldClose()) {

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
                    // Na Wayland nie działa (póki co)
                    break;
                case SDL_EVENT_WINDOW_RESTORED:                                         // przywrócenie okna z mimimalizacji
                    // Na Wayland nie działa (póki co)
                    break;
                case SDL_EVENT_WINDOW_MAXIMIZED:                                        // maksymalizacja okna
                    // ...
                    break;
                case SDL_EVENT_WINDOW_FOCUS_LOST:                                       // utrata fokusa --> robimy pauzę
                    m_paused = false;
                    break;
                case SDL_EVENT_WINDOW_FOCUS_GAINED:                                     // przywrócenie fokusa
                    m_paused = true;                                                    // ale co jeśli podczas utraty fokusa doszło np. do zmiany skali?
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

        // PAUSED
        if (!m_paused) {
            SDL_Delay(100);
            continue;
        }

        // RENDER
        m_graphics.Render(m_graphics.GetRenderer().GetDevice(), m_desc);

        // PSEUDO - FPS
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        timer += deltaTime;
        frames++;
        if (timer >= 0.5f) {
            std::cout << "FPS: " << frames * 2 << std::endl;
            frames = 0;
            timer = 0.0f;
        }

        // ASPECT RATIO
        if (actions.IsActionPressed(m_input, "Aspect")) {
            m_desc.ASPECT_RATIO = !m_desc.ASPECT_RATIO;
        }

        // FILTER
        if (actions.IsActionPressed(m_input, "Filter")) {
            if (m_desc.FILTER == TextureFilter::Nearest) {
                m_desc.FILTER = TextureFilter::Linear;
            } else {
                m_desc.FILTER = TextureFilter::Nearest;
            }

            vkDeviceWaitIdle(m_graphics.GetRenderer().GetDevice());

            RenderTarget* postTarget = nullptr;

            switch (m_desc.AA_MODE) {
                case AntiAliasing::SSAA:
                case AntiAliasing::SSAA_SMAA:
                    postTarget = &m_graphics.GetRenderer().GetSceneResources().SSAAColor;
                    break;
                case AntiAliasing::MSAA:
                case AntiAliasing::MSAA_SMAA:
                    postTarget = &m_graphics.GetRenderer().GetSceneResources().ResolveColor;
                    break;
                default:
                    postTarget = &m_graphics.GetRenderer().GetSceneResources().SceneColor;
                    break;
            }

            //m_graphics.GetRenderer().GetPostRenderPass().GetDescriptor().UpdateColor(m_graphics.GetRenderer().GetDevice(), *postTarget, m_desc.FILTER);
            //m_graphics.GetRenderer().GetSSAARenderPass().GetDescriptor().UpdateColor(m_graphics.GetRenderer().GetDevice(), m_graphics.GetRenderer().GetSceneResources().SceneColor, m_desc.FILTER);

            continue;

        }

        // VSYNC
        if (actions.IsActionPressed(m_input, "VSync")) {
            m_desc.VSYNC = !m_desc.VSYNC;
            m_graphics.GetRenderer().RecreateSwapchain(m_display, m_window, m_desc);
        }

        // WINDOWED
        if (actions.IsActionPressed(m_input, "Windowed")) {
            if (m_desc.FULLSCREEN) {
                m_desc.FULLSCREEN = false;
                m_window.SetWindowed(m_desc, m_display);
            } else {
                m_desc.FULLSCREEN = true;
                m_window.SetFullscreen(m_desc, m_display);
            }

            m_graphics.GetRenderer().RecreateRenderer(m_display, m_window, m_desc);
        }

        // SCREEN RESOLUTION
        int size = m_display.GetDisplayModes().size();
        std::vector<Mode> m = m_display.GetDisplayModes();
        auto it = std::find_if(m.begin(), m.end(),
            [&](const Mode& r) {
                return r.width == m_desc.WIDTH && r.height == m_desc.HEIGHT;
            });

        static int i = -1;
        if (it != m_display.GetDisplayModes().end()) {
            i = std::distance(m.begin(), it);
        }

        if (actions.IsActionPressed(m_input, "ResolutionDown")) {
            i++;
            if (i > (size-1)) i = (size-1);
            SetResolution(m[i]);
            std::cout << "Resolution set to: " << m[i].width << "x" << m[i].height << std::endl;
        }

        if (actions.IsActionPressed(m_input, "ResolutionUp")) {
            i--;
            if (i < 0) i = 0;
            SetResolution(m[i]);
            std::cout << "Resolution set to: " << m[i].width << "x" << m[i].height << std::endl;
        }

        // ANTIALIASING
        if (actions.IsActionPressed(m_input, "AA")) {
            switch (m_desc.AA_MODE) {
                case AntiAliasing::None:
                    m_desc.AA_MODE = AntiAliasing::MSAA;
                    m_desc.MSAA_SAMPLES = VK_SAMPLE_COUNT_16_BIT;
                    m_desc.SSAA_SCALE = 1.0f;
                    break;
                case AntiAliasing::MSAA:
                case AntiAliasing::MSAA_SMAA:
                    m_desc.AA_MODE = AntiAliasing::SSAA;
                    m_desc.MSAA_SAMPLES = VK_SAMPLE_COUNT_1_BIT;
                    m_desc.SSAA_SCALE = 2.0f;
                    break;
                case AntiAliasing::SSAA:
                case AntiAliasing::SSAA_SMAA:
                    m_desc.AA_MODE = AntiAliasing::SMAA;
                    m_desc.MSAA_SAMPLES = VK_SAMPLE_COUNT_1_BIT;
                    m_desc.SSAA_SCALE = 1.0f;
                    break;
                case AntiAliasing::SMAA:
                    m_desc.AA_MODE = AntiAliasing::None;
                    m_desc.MSAA_SAMPLES = VK_SAMPLE_COUNT_1_BIT;
                    m_desc.SSAA_SCALE = 1.0f;
                    break;
            }
            m_graphics.GetRenderer().RecreateRenderer(m_display, m_window, m_desc);
        }

        // TAKE SCREENSHOT
        if (actions.IsActionPressed(m_input, "Screenshot")) {
            m_desc.TAKE_SCREENSHOT = true;
        }

        // QUIT
        if (actions.IsActionPressed(m_input, "Quit")) break;

    }

    m_graphics.Shutdown(m_desc);
    m_window.Destroy();
    SDL_Quit();

}

void Application::SetResolution(const Mode& res) {

    // 1. Window desc
    m_desc.WIDTH = res.width;
    m_desc.HEIGHT = res.height;

    // 2. Zmienia rozmiar okna
    if (m_desc.FULLSCREEN) {
        // ...
    } else {
        m_window.SetWindowSize(m_desc, m_display);
    }

    // 3. Renderer ogarnia swapchain
    m_graphics.GetRenderer().RecreateRenderer(m_display, m_window, m_desc);

}

