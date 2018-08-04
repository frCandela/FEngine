#include "editor/Camera.h"

#include "glm/gtc/matrix_transform.hpp"

glm::mat4 Camera::GetView() const
{
	glm::mat4 view = glm::lookAt(GetGameobject()->GetTransform().position, GetGameobject()->GetTransform().position + GetGameobject()->GetTransform().Forward(), up);
	return view;
}

glm::mat4 Camera::GetProjection() const
{
	glm::mat4 proj = glm::perspective(glm::radians(fov), aspectRatio, nearp, farp);

	//the Y coordinate of the clip coordinates is inverted 
	proj[1][1] *= -1;

	return proj;
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