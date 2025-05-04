#pragma once

#include "Texture.h"
#include "System.h"
#include <unordered_map>
#include <string>

class TextureSystem : public System {
public:
    TextureSystem();
    ~TextureSystem();

    void loadTexture(Texture& texture);
    void bind(GLuint& textureHandler, unsigned int unit);

private:
    //GLuint currentTexture = 0; // Track the currently bound texture
};
