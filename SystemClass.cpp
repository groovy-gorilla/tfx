#include "SystemClass.h"

#include <iostream>


SystemClass::SystemClass() {
    window = nullptr;
    graphics = nullptr;
    input = nullptr;
}

SystemClass::~SystemClass() = default;

void SystemClass::Initialize() {
    InitializeWindow();

    graphics = new GraphicsClass();
    graphics->Initialize(window);

    input = new InputClass();
    input->Initialize();
}

void SystemClass::Shutdown() {
    graphics->Shutdown();
    delete graphics;
    graphics = nullptr;

    input->Shutdown();
    delete input;
    input = nullptr;

    ShutdownWindow();
}

void SystemClass::Run() {
    Loop();
}

void SystemClass::InitializeWindow() {

    setenv("GTK_THEME", "HighContrastInverse", 1);

    glfwInit();
    if (!glfwVulkanSupported()) {
        std::cerr << "GLFW Vulkan not supported!" << std::endl;
    }

    if (SETTINGS.DISPLAY_MODE == DISPLAY_MODE::WINDOWED) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
        glfwWindowHint(GLFW_PLATFORM_X11, GLFW_TRUE);
        window = glfwCreateWindow(SETTINGS.WIDTH, SETTINGS.HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if (SETTINGS.DISPLAY_MODE == DISPLAY_MODE::FULLSCREEN) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE , GLFW_FALSE);
        glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        window = glfwCreateWindow(SETTINGS.WIDTH, SETTINGS.HEIGHT, "Vulkan", monitor, nullptr);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void SystemClass::ShutdownWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void SystemClass::Loop() {
    while (!glfwWindowShouldClose(window)) {

        graphics->vulkan->DrawFrame(window);

        glfwPollEvents();
        input->BeginProcessInput(window);

        // SCREEN RESOLUTION
        if (input->IsPressed(GLFW_KEY_EQUAL)) {
            graphics->vulkan->SetResolution(3440, 1440);
        }

        if (input->IsPressed(GLFW_KEY_MINUS)){
            graphics->vulkan->SetResolution(320, 240);
        }

        // MSAA
        static bool msaa = false;
        if (input->IsPressed(GLFW_KEY_M)) {
            if (msaa) {
                graphics->vulkan->SetMSAA(VK_SAMPLE_COUNT_1_BIT);
                msaa = false;
            } else {
                graphics->vulkan->SetMSAA(VK_SAMPLE_COUNT_16_BIT);
                msaa = true;
            }
        }

        // FILTER
        static bool filter = false;
        if (input->IsPressed(GLFW_KEY_F)) {
            if (filter) {
                graphics->vulkan->SetFilter(VK_FILTER_NEAREST);
                filter = false;
            } else {
                graphics->vulkan->SetFilter(VK_FILTER_LINEAR);
                filter = true;
            }
        }

        // ASPECT RATIO
        static bool aspect = false;
        if (input->IsPressed(GLFW_KEY_A)) {
            if (aspect) {
                graphics->vulkan->SetAspectRatioEnabled(false);
                aspect = false;
            } else {
                graphics->vulkan->SetAspectRatioEnabled(true);
                aspect = true;
            }
        }

        // QUIT
        if (input->IsPressed(GLFW_KEY_ESCAPE)) {
            break;
        }

        input->EndProcessInput();

    }

    vkDeviceWaitIdle(graphics->vulkan->GetDevice());
}
