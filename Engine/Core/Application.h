#pragma once

#include "Display.h"
#include "Window.h"
#include "../../Engine/Input/Input.h"
#include "../../Engine/Input/InputMapping.h"
#include "../../Graphics/Graphics.h"

class Application {
public:
    void Run();

private:
    bool m_running = true;

    Display m_display;
    Window m_window;
    Input m_input;
    InputMapping m_inputMapping;
    Graphics m_graphics;

};