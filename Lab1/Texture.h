#pragma once

#include <string>
#include <GL/glew.h>

struct Texture {
    std::string texturePath; // Path to the texture file
    GLuint textureHandler = 0; // OpenGL texture ID

    Texture() = default;

    Texture(const std::string& texturePath)
        : texturePath(texturePath) {
    }
};
