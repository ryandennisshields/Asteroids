#pragma once

#include "System.h"
#include "Transform.h"
#include "Coordinator.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

struct TransformSystem : public System
{
public:
	inline glm::mat4 update(Entity entity) const
	{
		auto& transform = coordinator.getComponent<Transform>(entity); // Get the entity's transform component
		glm::mat4 posMat = glm::translate(transform.position);
		glm::mat4 scaleMat = glm::scale(transform.scale);
		glm::mat4 rotX = glm::rotate(transform.rotation.x, glm::vec3(1.0, 0.0, 0.0));
		glm::mat4 rotY = glm::rotate(transform.rotation.y, glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 rotZ = glm::rotate(transform.rotation.z, glm::vec3(0.0, 0.0, 1.0));
		glm::mat4 rotMat = rotX * rotY * rotZ;

		return posMat * rotMat * scaleMat;
	}
};


