#include "MeshSystem.h"

void MeshSystem::loadModel(Mesh& mesh) {
    IndexedModel model = OBJModel(mesh.modelPath).ToIndexedModel();
    initModel(mesh, model);
}

void MeshSystem::initModel(Mesh& mesh, const IndexedModel& model) {
    mesh.drawCount = model.indices.size();

    if (model.positions.empty() || model.texCoords.empty() || model.normals.empty()) {
        std::cerr << "ERROR: Model data is missing!" << std::endl;
        return;
    }

    // Create Buffers
    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glGenBuffers(1, &mesh.ebo);

    // Create Interleaved Data
    std::vector<float> interleavedData;
    for (size_t i = 0; i < model.positions.size(); i++) {
        interleavedData.push_back(model.positions[i].x);
        interleavedData.push_back(model.positions[i].y);
        interleavedData.push_back(model.positions[i].z);

        interleavedData.push_back(model.normals[i].x);
        interleavedData.push_back(model.normals[i].y);
        interleavedData.push_back(model.normals[i].z);

        interleavedData.push_back(model.texCoords[i].x);
        interleavedData.push_back(model.texCoords[i].y);
    }

    glBindVertexArray(mesh.vao);

    // Interleaved Data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, interleavedData.size() * sizeof(float), interleavedData.data(), GL_STATIC_DRAW);

    // Index Data to EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(unsigned int), model.indices.data(), GL_STATIC_DRAW);

    // Vertex Attributes
    size_t stride = sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2);

    glEnableVertexAttribArray(0); // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    glEnableVertexAttribArray(1); // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)));

    glEnableVertexAttribArray(2); // Texcoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec3)));

    glBindVertexArray(0);
}

void MeshSystem::render(const Mesh& mesh) {
    // Bind the VAO and set the model matrix
    glBindVertexArray(mesh.vao);
    // Draw the mesh
    glDrawElements(GL_TRIANGLES, mesh.drawCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
