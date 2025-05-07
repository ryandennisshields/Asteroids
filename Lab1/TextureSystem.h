#pragma once

#include "Texture.h"
#include "System.h"
#include <unordered_map>
#include <string>

class TextureSystem : public System {
public:
    TextureSystem() = default;
    ~TextureSystem() = default;

	void loadTexture(Texture& texture); // Load a texture from file
	void Bind(GLuint& textureHandler); // Bind a texture to a texture unit
};
