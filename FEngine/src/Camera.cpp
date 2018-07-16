#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"

glm::mat4 Camera::GetView() const
{
	glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	return view;
}

glm::mat4 Camera::GetProj() const
{
	glm::mat4 proj = glm::perspective(glm::radians(fov), aspectRatio, near, far);

	//the Y coordinate of the clip coordinates is inverted 
	proj[1][1] *= -1;

	return proj;
}