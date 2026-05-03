#pragma once

#include "Display.h"
#include "Window.h"
#include "ApplicationDesc.h"
#include "../../Engine/Input/Input.h"
#include "../../Engine/Input/InputMapping.h"
#include "../../Engine/Platform/SDL/SDLInput.h"
#include "../../Graphics/Graphics.h"

class Application {
public:
    void Run();

    void SetResolution(const Mode& res);

private:
    bool m_running = true;
    bool m_pendingFullscreen = true;

    ApplicationDesc m_desc;
    Display m_display;
    Window m_window;
    Input m_input;
    SDLInput m_sdlInput;
    InputMapping actions;
    Graphics m_graphics;

};