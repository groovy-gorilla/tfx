#pragma once
#include "Texture.h"

class Bitmap {

public:
    Bitmap();
    ~Bitmap();

    void Initialize(VkDevice& device, VkPhysicalDevice& physicalDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, const std::string& filename);
    void Shutdown(VkDevice& device);

    Texture& GetTexture();

private:
    Texture* m_texture;

};