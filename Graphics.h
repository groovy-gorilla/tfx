#pragma once
#include "Vulkan.h"
#include "Bitmap.h"
#include "BitmapRenderer.h"

class Graphics {

public:
    Graphics();
    ~Graphics();
    void Initialize(SDL_Window* window);
    void Shutdown();

    void Draw(SDL_Window* window);

    Vulkan* m_vulkan;

private:

    Bitmap* m_bitmap;
    BitmapRenderer* m_bitmapRenderer;

};

