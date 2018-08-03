#pragma once

#include "editor/Component.h"

#include <glm/glm.hpp>


class Camera : public Component
{
public:
	bool IsUnique() const override { return true; }

	glm::mat4 GetView() const;
	glm::mat4 GetProj() const;

	glm::vec3 pos = { 20, 0,0 };
	glm::vec3 dir = glm::vec3( -1, 0, 0);

	float fov = 90.f;
	float aspectRatio = 1.f;
	float nearp = 0.1f;
	float farp = 100.f;
};