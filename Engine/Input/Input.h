#pragma once
#include "KeyCodes.h"
#include <cstdint>
#include <vector>

class Input {

public:
    Input();
    ~Input();

    void Initialize(int keyCount);

    void BeginFrame();
    void SetKeyState(Key key, bool isDown);

    // KEYBOARD
    bool IsKeyPressed(Key key) const;
    bool IsKeyDown(Key key) const;
    bool IsKeyReleased(Key key) const;

private:
    int keyCount = 0;

    std::vector<uint8_t> currentKeys;
    std::vector<uint8_t> prevKeys;

};
