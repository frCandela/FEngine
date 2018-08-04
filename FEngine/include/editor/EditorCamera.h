#pragma once

#include "Camera.h"

struct EditorCamera : public Camera
{
	std::string GetName() const override;
	void Update(float delta);
	void RenderGui() override;

	float speed = 20.f;
	float speedMultiplier = 3.f;
	float xSensitivity = 1.f;
};