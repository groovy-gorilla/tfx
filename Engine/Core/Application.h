#pragma once

#include "ApplicationDesc.h"
#include "Display.h"
#include "Timer.h"
#include "Window.h"
#include "Input/Input.h"
#include "Input/InputMapping.h"
#include "Platform/SDL/SDLInput.h"
#include "Graphics/Graphics.h"

class Display;

class Application {
public:
    void Run();

    void SetResolution(const Mode& res);

private:
    bool m_running = true;
    bool m_paused = true;

    ApplicationDesc m_desc;
    Display m_display;
    Window m_window;
    Timer m_timer;
    Input m_input;
    SDLInput m_sdlInput;
    InputMapping actions;
    Graphics m_graphics;

};