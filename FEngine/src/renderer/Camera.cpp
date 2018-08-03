#include "renderer/Camera.h"

#include "glm/gtc/matrix_transform.hpp"



glm::mat4 Camera::GetView() const
{
	glm::mat4 view = glm::lookAt(position, position + Forward(), up);
	return view;
}

glm::mat4 Camera::GetProjection() const
{
	glm::mat4 proj = glm::perspective(glm::radians(fov), aspectRatio, nearp, farp);

	//the Y coordinate of the clip coordinates is inverted 
	proj[1][1] *= -1;

	return proj;
}