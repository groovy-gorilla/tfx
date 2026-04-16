#include "InputClass.h"

#include <cstring>
#include <GLFW/glfw3.h>

InputClass::InputClass() {

    memset(prev, 0, sizeof(prev));
    memset(curr, 0, sizeof(curr));

}

InputClass::~InputClass() = default;

void InputClass::Initialize() {

}

void InputClass::Shutdown() {

}

void InputClass::BeginProcessInput(GLFWwindow* window) {

    for (int i = 0; i < GLFW_KEY_LAST; ++i) {
        curr[i] = glfwGetKey(window, i) == GLFW_PRESS;
    }

}

void InputClass::EndProcessInput() {

    memcpy(prev, curr, sizeof(curr));

}

bool InputClass::IsPressed(int key) {
    if (key < 0 || key >= GLFW_KEY_LAST) return false;
    return curr[key] && !prev[key];
}

bool InputClass::IsHeld(int key) {
    return curr[key];
}

bool InputClass::IsReleased(int key) {
    return !curr[key] && prev[key];
}


