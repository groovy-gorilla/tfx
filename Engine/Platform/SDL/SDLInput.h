#pragma once

#include <SDL3/SDL.h>
#include "../../../Engine/Input/Input.h"

class SDLInput {
public:
    SDLInput(Input& input);

    void ProcessEvent(const SDL_Event& e);

private:
    Input& m_Input;
};