#pragma once

#include <glm/glm.hpp>
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"

#include <iostream>

class Transform
{
public:

	void SetPosition(glm::vec3 newPosition);
	void SetScale(glm::vec3 newScale);
	void SetRotation(glm::quat newRotation);

	glm::vec3 GetPosition() const;
	glm::vec3 GetScale() const;
	glm::quat GetRotation() const;

	glm::vec3 Right() const;
	glm::vec3 Forward() const;
	glm::vec3 Up() const;
	glm::mat4 GetModelMatrix() const;
	
	void RenderGui();

private:

	glm::quat m_rotation = glm::quat(0, 0, 0, 1);	// Rotation in radians
	glm::vec3 m_position = glm::vec3(0, 0, 0);
	glm::vec3 m_scale = glm::vec3(1, 1, 1);
};
