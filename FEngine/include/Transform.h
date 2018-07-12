#pragma once

class Transform : public Component
{
public:
	bool IsUnique() const override { return false; }
};
