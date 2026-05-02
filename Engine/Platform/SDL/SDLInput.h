#pragma once

#include <SDL3/SDL.h>
#include "../../../Engine/Input/Input.h"

class SDLInput {
public:
    void Initialize(Input* input);

    void ProcessEvent(const SDL_Event& e);

private:
    Input* m_input = nullptr;
};