#pragma once

#include "editor/Component.h"

#include <glm/glm.hpp>


class Camera : public Component
{
public:
	bool IsUnique() const override { return true; }

	glm::mat4 GetView() const;
	glm::mat4 GetProj() const;

	float fov = 45.f;
	float aspectRatio = 1.f;
	float nearp = 0.1f;
	float farp = 100.f;
};