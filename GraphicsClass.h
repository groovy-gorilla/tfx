#ifndef GRAPHICSCLASS_H
#define GRAPHICSCLASS_H
#include <GLFW/glfw3.h>

#include "VulkanClass.h"

class GraphicsClass {

public:
    GraphicsClass();
    ~GraphicsClass();
    void Initialize(GLFWwindow *window);
    void Shutdown();

    VulkanClass *vulkan;

private:


};

#endif //GRAPHICSCLASS_H