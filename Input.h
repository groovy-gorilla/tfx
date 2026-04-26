#pragma once
#include <SDL3/SDL.h>
#include <vector>

class Input {

public:
    Input();
    ~Input();

    void Initialize();

    void BeginFrame();
    void EndFrame();

    // KEYBOARD
    bool IsKeyPressed(SDL_Scancode key) const;
    bool IsKeyDown(SDL_Scancode key) const;
    bool IsKeyReleased(SDL_Scancode key) const;

private:
    int keyCount = 0;

    const bool* currentKeys = nullptr;
    std::vector<uint8_t> prevKeys;

};
