#pragma once

#include "Display.h"
#include "Window.h"
#include "../../Engine/Input/Input.h"
#include "../../Engine/Input/InputMapping.h"
#include "../../Engine/Platform/SDL/SDLInput.h"

class Application {
public:
    void Run();

private:
    bool m_running = true;

    Input m_input;
    InputMapping m_inputMapping;
    Display m_display;
    Window m_window;

};