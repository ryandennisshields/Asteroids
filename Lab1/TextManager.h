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
#include "Texture.h"
#include "Camera.h"
#include "UBOManager.h"

class TextManager
{
public:
    static TextManager& getInstance();

	void renderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color); // Show text on screen

private:
	TextManager();
    ~TextManager() = default;
    TextManager(const TextManager&) = delete;
    TextManager& operator=(const TextManager&) = delete;

	struct Character {
		GLuint TextureID;   // ID handle of the glyph texture
		glm::ivec2 Size;    // Size of glyph
		glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
		GLuint Advance;     // Offset to advance to next glyph
	};

	std::map<GLchar, Character> Characters; // Holds all characters

	unsigned int VAO, VBO; // Holds VAO and VBO for text
};