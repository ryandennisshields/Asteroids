//#pragma once
//
//#include "Mesh.h"
//#include "Transform.h"
//#include "Shader.h"
//
//struct GameObject {
//    Mesh* mesh = nullptr;            // Pointer to a shared mesh
//    Transform* transform = nullptr;  // Pointer to a shared transform 
//    Shader* shader = nullptr;        // Pointer to a shared shader
//
//    glm::vec3 forwardDirection = glm::vec3(0, 1, 0);  // Default forward (Y-axis)
//    glm::vec3 velocity = glm::vec3(0);               // Initial velocity
//
//    // Default constructor allows empty object creation
//    GameObject() = default;
//
//    // Constructor-based initialization
//    GameObject(Mesh* m, Transform* t, Shader* s)
//        : mesh(m), transform(t), shader(s) {}
//
//    // Flexible init function (can be called manually)
//    void init(Mesh* m, Transform* t, Shader* s) {
//        if (!mesh) mesh = m;
//        if (!transform) transform = t;
//        if (!shader) shader = s;
//    }
//};
