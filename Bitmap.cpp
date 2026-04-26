#include "Bitmap.h"

#include <stdexcept>

Bitmap::Bitmap() {

    m_texture = nullptr;

}

Bitmap::~Bitmap() = default;

void Bitmap::Initialize(VkDevice& device, VkPhysicalDevice& physicalDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue, const std::string& filename) {

    m_texture = new Texture;
    if (!m_texture) {
        throw std::runtime_error("Bitmap: texture not initialized!");
    }

    m_texture->Initialize(device, physicalDevice, commandPool, graphicsQueue, filename, TextureType::Bitmap);



}

void Bitmap::Shutdown(VkDevice& device) {

    if (m_texture) {
        m_texture->Shutdown(device);
        delete m_texture;
        m_texture = nullptr;
    }

}

Texture& Bitmap::GetTexture() {

    return *m_texture;

}


