#pragma once

#include "Mesh.h"
#include "Coordinator.h"
#include "obj_loader.h"
#include <iostream>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

class MeshSystem : public System {
public:
    MeshSystem() = default;
    ~MeshSystem() = default;

    void loadModel(Mesh& mesh); // Load a model and initialize it
    void render(const Mesh& mesh); // Render a mesh

private:
    void initModel(Mesh& mesh, const IndexedModel& model); // Initialize a model
};

