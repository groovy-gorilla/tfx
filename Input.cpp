#include "Input.h"

Input::Input() {}

Input::~Input() = default;

void Input::Initialize() {

    currentKeys = SDL_GetKeyboardState(&keyCount);
    prevKeys.resize(keyCount, 0);

}

void Input::BeginFrame() {

    memcpy(prevKeys.data(), currentKeys, keyCount);

}

void Input::EndFrame() {

    currentKeys = SDL_GetKeyboardState(nullptr);

}

bool Input::IsKeyPressed(SDL_Scancode key) const {
    return currentKeys[key] && !prevKeys[key];
}

bool Input::IsKeyDown(SDL_Scancode key) const {
    return currentKeys[key];
}

bool Input::IsKeyReleased(SDL_Scancode key) const {
    return !currentKeys[key] && prevKeys[key];
}


