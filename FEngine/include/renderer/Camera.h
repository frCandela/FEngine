#pragma once

#include "editor/Component.h"

#include <glm/glm.hpp>
#include "util/Input.h"


struct Camera : public Component
{
	bool IsUnique() const override { return true; }

	glm::mat4 GetView() const;
	glm::mat4 GetProjection() const;

	glm::vec3 up = { 0,1,0 };
	glm::vec3 position = { 0, 0,0 };
	glm::quat rotation = {0,0,0,1};


	glm::vec3 Right() const { return  rotation * glm::vec3(1.f, 0, 0); }
	glm::vec3 Forward() const { return  glm::normalize(rotation * glm::vec3(0,0,1)); }
	glm::vec3 Up() const { return rotation * glm::vec3(0, -1, 0); } //??

	float fov = 90.f;
	float aspectRatio = 1.f;
	float nearp = 0.1f;
	float farp = 100.f;
};

struct FPSCamera : public Camera
{
	float speed = 20.f;
	float speedMultiplier = 3.f;

	float xSensitivity = 1.f;

	void Update(float delta)
	{
		// Calculates speed
		float realSpeed = speed;
		if (Keyboard::KeyDown(GLFW_KEY_LEFT_SHIFT))
			realSpeed *= speedMultiplier;

		// Camera goes right
		float rightAxis = 0.f;
		if (Keyboard::KeyDown(GLFW_KEY_D))
			rightAxis += 1.f;			
		else if (Keyboard::KeyDown(GLFW_KEY_A))
			rightAxis -= 1.f;
		position += delta * realSpeed * rightAxis * Right();

		// Camera goes up
		float upAxis = 0.f;
		if (Keyboard::KeyDown(GLFW_KEY_E))
			upAxis += 1.f;
		else if (Keyboard::KeyDown(GLFW_KEY_Q))
			upAxis -= 1.f;
		position += delta * realSpeed * upAxis * Up();

		// Camera goes forward
		float forwardAxis = 0.f;
		if (Keyboard::KeyDown(GLFW_KEY_W))
			forwardAxis += 1.f;
		else if (Keyboard::KeyDown(GLFW_KEY_S))
			forwardAxis -= 1.f;
		position += delta * realSpeed * forwardAxis * Forward();		


		if (Mouse::KeyDown(Mouse::button1))
		{
			glm::vec2 mouseDelta = Mouse::Delta();

			glm::quat yaw = glm::angleAxis( - delta * xSensitivity * mouseDelta.x, up);
			glm::quat pitch = glm::angleAxis( - delta * xSensitivity * mouseDelta.y, Right());
			rotation =  yaw * pitch * rotation;
		}
	}
};