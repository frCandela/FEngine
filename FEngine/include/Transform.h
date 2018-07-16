#pragma once

#include "Component.h"

class Transform : public Component
{
public:
	bool IsUnique() const override { return false; }
};
