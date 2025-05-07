#pragma once

#include <string>
#include <GL/glew.h>

struct Texture {
    GLuint textureHandler = 0; // OpenGL texture ID
    std::string texturePath; // Path to the texture file

    Texture() = default;

    Texture(const std::string& texturePath)
        : texturePath(texturePath) {
    }
};
