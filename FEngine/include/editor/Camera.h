#pragma once

#include "editor/Component.h"

#include <glm/glm.hpp>
#include "util/Input.h"

struct Camera : public Component
{
	bool IsUnique() const override { return true; }
	std::string GetName() const override { return "camera"; }
	void RenderGui() override;

	glm::mat4 GetView() const;
	glm::mat4 GetProjection() const;

	glm::vec3 up = { 0,1,0 };
	float fov = 90.f;
	float aspectRatio = 1.f;
	float nearp = 0.1f;
	float farp = 100.f;
};