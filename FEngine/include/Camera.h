#pragma once

#include "Component.h"

#include <glm/glm.hpp>


class Camera : public Component
{
public:
	bool IsUnique() const override { return true; }

private:
	glm::mat4 view;
	glm::mat4 proj;
};