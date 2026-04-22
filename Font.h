#ifndef FONTCLASS_H
#define FONTCLASS_H
#include "Texture.h"

struct FontType {
    float u0, v0;
    float u1, v1;
};

class Font {

public:
    Font();
    ~Font();

    void Initialize(Texture* texture);
    void Shutdown();

private:
    Texture* m_texture;


};

#endif //FONTCLASS_H
