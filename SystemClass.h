#ifndef SYSTEMCLASS_H
#define SYSTEMCLASS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "GraphicsClass.h"
#include "InputClass.h"
#include "Settings.h"

class SystemClass {

public:
    SystemClass();
    ~SystemClass();
    void Initialize();
    void Shutdown();
    void Run();

private:
    GLFWwindow *window;
    GraphicsClass *graphics;
    InputClass *input;

    void InitializeWindow();
    void ShutdownWindow();

    void Loop();

};

#endif //SYSTEMCLASS_H