#pragma once

#include "Input/Input.h"

class SDLInput {
public:
    void Initialize(Input* input);

    void ProcessEvent(const SDL_Event& e);

private:
    Input* m_input = nullptr;
};