#pragma once
#include "Graphics.h"
#include "Engine/Input/Input.h"
#include "Engine/Input/InputMapping.h"
#include <SDL3/SDL.h>

//#include "Platform/SDL/SDLInput.h"

class System {

public:
    System();
    ~System();
    void Initialize();
    void Shutdown();
    void Run();

private:
    SDL_Window *m_window;
    SDL_DisplayID* m_displays;
    SDL_DisplayMode* m_currentDisplayMode;
    Graphics* m_graphics;
    float m_scaling;

    void InitializeWindow();
    void ShutdownWindow();

    void Loop();

};


