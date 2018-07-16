#pragma once


#include <glm/glm.hpp>
#include "glm/gtc/quaternion.hpp"

class Transform
{
public:
	glm::quat rotation;
	glm::vec3 position;
};
