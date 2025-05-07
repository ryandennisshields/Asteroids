#include "TextureSystem.h"
#include "stb_image.h"
#include <iostream>
#include <cassert>

void TextureSystem::loadTexture(Texture& texture) {
    int width, height, numComponents;
    unsigned char* imageData = stbi_load(texture.texturePath.c_str(), &width, &height, &numComponents, 4);

    if (imageData == nullptr) {
        std::cerr << "[ERROR] Texture load failed: " << texture.texturePath << std::endl;
        return;
    }

    glGenTextures(1, &texture.textureHandler);
    glBindTexture(GL_TEXTURE_2D, texture.textureHandler);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

    stbi_image_free(imageData);
}

void TextureSystem::Bind(GLuint& textureHandler) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureHandler);
}
