#pragma once

#include "Mesh.h"
#include "Coordinator.h"
#include "obj_loader.h"
#include "UBOManager.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

class MeshSystem : public System {
public:
    MeshSystem();
    ~MeshSystem();

    void loadModel(Mesh& mesh); // Load a model and initialize its OpenGL buffers
    void render(const Mesh& mesh); // Render a single mesh

private:
    void initModel(Mesh& mesh, const IndexedModel& model); // Initialize OpenGL buffers for a model
};

