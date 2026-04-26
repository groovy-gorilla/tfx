#include "Font.h"
#include <stdexcept>

Font::Font() {

    m_texture = nullptr;

}

Font::~Font() = default;

void Font::Initialize(Texture* texture) {

    if (!texture) throw std::runtime_error("Texture is nullptr");
    m_texture = texture;

}

void Font::Shutdown() {



}



