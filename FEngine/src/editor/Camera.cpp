#include "editor/Camera.h"

#include "glm/gtc/matrix_transform.hpp"

glm::mat4 Camera::GetView() const
{
	Transform* transform = GetGameobject()->GetComponent<Transform>();
	glm::mat4 view = glm::lookAt(transform->GetPosition(), transform->GetPosition() + transform->Forward(), up);
	return view;
}

glm::mat4 Camera::GetProjection() const
{
	glm::mat4 proj = glm::perspective(glm::radians(fov), aspectRatio, nearp, farp);

	//the Y coordinate of the clip coordinates is inverted 
	proj[1][1] *= -1;

	return proj;
}

Ray Camera::ScreenPosToRay(glm::vec2 position)
{
	assert(position.x >= -1.f  && position.x <= 1.f);
	assert(position.y >= -1.f && position.y <= 1.f);

	Transform* transform = GetGameobject()->GetComponent<Transform>();

	const glm::vec3 pos = transform->GetPosition();
	const glm::vec3 up = transform->Up();
	const glm::vec3 right = transform->Right();
	const glm::vec3 forward = transform->Forward();

	glm::vec3 nearMiddle = pos + nearp * forward;

	float nearHeight = nearp * tan(glm::radians(fov / 2));
	float nearWidth = aspectRatio * nearHeight;

	Ray ray;
	ray.origin = nearMiddle + position.x * nearWidth * right - position.y * nearHeight * up;
	ray.direction = 100.f * glm::normalize(ray.origin - pos);

	return ray;
}

void Camera::RenderGui()
{
	Component::RenderGui();
	ImGui::DragFloat("fov", &fov, 1.f, 0.f, 180.f);
	ImGui::DragFloat("nearp", &nearp, 0.025f, 0.f, std::numeric_limits<float>::max());
	ImGui::DragFloat("farp", &farp, 1.f, 0.f, std::numeric_limits<float>::max());

	float aspectRatio = 1.f;
	float nearp = 0.1f;
	float farp = 100.f;
}