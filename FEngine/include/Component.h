#pragma once

class Component
{
public:
	///Returns true is the component is Unique (only one instance per GameObject) false otherwise
	virtual bool IsUnique() const = 0;
};
