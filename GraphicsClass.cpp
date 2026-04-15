#include "GraphicsClass.h"

GraphicsClass::GraphicsClass() {
    vulkan = nullptr;
}

GraphicsClass::~GraphicsClass() = default;

void GraphicsClass::Initialize(GLFWwindow *window) {
    vulkan = new VulkanClass;
    vulkan->Initialize(window);
}

void GraphicsClass::Shutdown() {
    vulkan->Shutdown();
    delete vulkan;
    vulkan = nullptr;
}
