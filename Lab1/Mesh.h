#pragma once

#include <GL/glew.h>
#include <string>

struct Mesh {
    GLuint vao = 0;  // Vertex Array Object
    GLuint vbo = 0;  // Vertex Buffer Object
    GLuint ebo = 0;  // Element Buffer Object
    unsigned int drawCount = 0; // Number of indices
    std::string modelPath; // Path to the model file

    Mesh() = default;

    Mesh(const std::string& modelPath)
        : modelPath(modelPath) {
    }
};

