#include "renderer/Camera.h"

#include "glm/gtc/matrix_transform.hpp"

glm::mat4 Camera::GetView() const
{
	glm::mat4 view = glm::lookAt(glm::vec3(20, 2, 2), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.f));
	return view;
}

glm::mat4 Camera::GetProj() const
{
	glm::mat4 proj = glm::perspective(glm::radians(fov), aspectRatio, nearp, farp);

	//the Y coordinate of the clip coordinates is inverted 
	proj[1][1] *= -1;

	return proj;
}