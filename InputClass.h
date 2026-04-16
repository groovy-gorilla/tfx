#ifndef INPUTCLASS_H
#define INPUTCLASS_H
#include <GLFW/glfw3.h>

class InputClass {

public:
    InputClass();
    ~InputClass();
    void Initialize();
    void Shutdown();
    void BeginProcessInput(GLFWwindow* window);
    void EndProcessInput();
    bool IsPressed(int key);
    bool IsHeld(int key);
    bool IsReleased(int key);

private:
    bool prev[GLFW_KEY_LAST] = {};
    bool curr[GLFW_KEY_LAST] = {};

};

#endif //INPUTCLASS_H