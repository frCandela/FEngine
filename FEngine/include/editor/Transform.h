#pragma once

#include <glm/glm.hpp>
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"

#include <iostream>

class Transform
{
public:
	glm::quat rotation = glm::quat(0,0,0,1);	// Rotation in radians
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 scale = glm::vec3(1, 1, 1);

	glm::vec3 Right() const;
	glm::vec3 Forward() const;
	glm::vec3 Up() const;
	glm::mat4 GetModelMatrix() const;
	
	void RenderGui();
};
