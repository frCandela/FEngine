#pragma once

#include "editor/Component.h"
#include "editor/Transform.h"

#include <glm/glm.hpp>

#include "util/UserInput.h"
#include "Shape.h"

struct Camera : public Component
{
	bool IsUnique() const override { return true; }
	std::string GetName() const override { return "camera"; }
	void RenderGui() override;

	glm::mat4 GetView() const;
	glm::mat4 GetProjection() const;

	// Returns a ray going from camera through a screen point ( with screenSpacePosition between {-1.f,-1.f} and {1.f,1.f} ).
	Ray ScreenPosToRay( glm::vec2 screenSpacePosition );

	glm::vec3 up = { 0,1,0 };
	float fov = 90.f;
	float aspectRatio = 1.f;
	float nearp = 0.1f;
	float farp = 1000.f;
};