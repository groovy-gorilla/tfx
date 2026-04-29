#include "Input.h"
#include <cstring>

Input::Input() {}
Input::~Input() = default;

void Input::Initialize(int count) {
    keyCount = count;

    currentKeys.resize(keyCount, 0);
    prevKeys.resize(keyCount, 0);
}

void Input::BeginFrame() {
    memcpy(prevKeys.data(), currentKeys.data(), keyCount);
}

void Input::SetKeyState(Key key, bool isDown) {
    int k = static_cast<int>(key);
    if (k >= keyCount) return;
    currentKeys[k] = isDown ? 1 : 0;
}

bool Input::IsKeyPressed(Key key) const {
    int k = static_cast<int>(key);
    if (k >= keyCount) return false;
    return currentKeys[k] && !prevKeys[k];
}

bool Input::IsKeyDown(Key key) const {
    int k = static_cast<int>(key);
    if (k >= keyCount) return false;
    return currentKeys[k];
}

bool Input::IsKeyReleased(Key key) const {
    int k = static_cast<int>(key);
    if (k >= keyCount) return false;
    return !currentKeys[k] && prevKeys[k];
}


