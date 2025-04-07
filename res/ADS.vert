#version 400 core

layout(location = 0) in vec3 position;  // Vertex Position (x, y, z)
layout(location = 1) in vec3 normal;    // Normal Vector (x, y, z)
layout(location = 2) in vec2 texcoord;  // Texture Coordinates (u, v)

layout(std140) uniform Matrices {
    mat4 model;
    mat4 view;
    mat4 projection;
};

uniform float angle; // Rotation angle in radians

out vec2 fragTexcoord;
out vec3 fragNormal;
out vec3 fragPosition;

void main() {
    // Rotation matrix around the Y-axis
    mat4 rotationY = mat4(
        cos(angle), 0.0, sin(angle), 0.0,
        0.0,       1.0, 0.0,       0.0,
       -sin(angle), 0.0, cos(angle), 0.0,
        0.0,       0.0, 0.0,       1.0
    );

    // Apply rotation before model transformation
    vec4 worldPosition = model * rotationY * vec4(position, 1.0);

    gl_Position = projection * view * worldPosition;

    fragTexcoord = texcoord;
    fragNormal = normalize(mat3(transpose(inverse(model * rotationY))) * normal); // Transform normals correctly
    fragPosition = worldPosition.xyz;
}
