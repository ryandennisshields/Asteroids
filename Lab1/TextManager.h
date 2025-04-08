//
//	This manager makes use of the FreeType free online library to load fonts and render text. FreeType can be found here:
//	https://www.freetype.org/
//
//  This tutorial was also used:
//  https://learnopengl.com/In-Practice/Text-Rendering
//
#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>
#include "Shader.h"

class TextManager {
public:
    // Singleton pattern
    static TextManager& getInstance();

    void init();

    void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color);

private:
    TextManager();
    ~TextManager() = default;

    unsigned int VAO, VBO;

    struct Character {
        unsigned int TextureID;  // ID handle of the glyph texture
        glm::ivec2   Size;       // Size of glyph
        glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };
    std::map<char, Character> Characters;

    // Prevent copy/move
    TextManager(const TextManager&) = delete;
    TextManager& operator=(const TextManager&) = delete;
};