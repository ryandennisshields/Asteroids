#pragma once

#include "Mesh.h"
#include <glm\glm.hpp>
#include <GL\glew.h>
#include <string>
#include "obj_loader.h"
#include <iostream>
#include <vector>

class MeshSystem {
public:
	MeshSystem();
	~MeshSystem();

	void draw();
	void init(Mesh* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices);
	void loadModel(const std::string& filename);
	void initModel(const IndexedModel& model);

	GLuint vao = 0;  // Vertex Array Object
private:
	GLuint vbo = 0;  // Interleaved Vertex Buffer Object
	GLuint ebo = 0;  // Index Buffer Object
	unsigned int drawCount = 0; // Number of indices
};
