#include "SDLInput.h"

Key TranslateKey(SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_W: return Key::W;
        case SDL_SCANCODE_A: return Key::A;
        case SDL_SCANCODE_F: return Key::F;
        case SDL_SCANCODE_M: return Key::M;

        case SDL_SCANCODE_EQUALS: return Key::Equals;
        case SDL_SCANCODE_MINUS: return Key::Minus;

        case SDL_SCANCODE_ESCAPE: return Key::Escape;

        default: return Key::Unknown;
    }
}

SDLInput::SDLInput(Input& input)
    : m_Input(input) {}

void SDLInput::ProcessEvent(const SDL_Event& e) {
    if (e.type == SDL_EVENT_KEY_DOWN) {
        Key key = TranslateKey(e.key.scancode);
        m_Input.SetKeyState(key, true);
    }
    else if (e.type == SDL_EVENT_KEY_UP) {
        Key key = TranslateKey(e.key.scancode);
        m_Input.SetKeyState(key, false);
    }
}




