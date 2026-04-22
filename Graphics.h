#ifndef GRAPHICSCLASS_H
#define GRAPHICSCLASS_H
#include <GLFW/glfw3.h>
#include "Vulkan.h"
#include "Bitmap.h"
#include "BitmapRenderer.h"


class Graphics {

public:
    Graphics();
    ~Graphics();
    void Initialize(GLFWwindow *window);
    void Shutdown();

    void Draw(GLFWwindow *window);

    Vulkan* m_vulkan;

private:

    Bitmap* m_bitmap;
    BitmapRenderer* m_bitmapRenderer;

};

#endif //GRAPHICSCLASS_H