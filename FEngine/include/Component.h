#pragma once

class GameObject;

class Component
{
public:
	friend class GameObject;

	///Returns true is the component is Unique (only one instance per GameObject) false otherwise
	virtual bool IsUnique() const = 0;

	///Returns a reference on the Gameobject the component is bound to
	inline GameObject& Gameobject() const { return *m_pGameobject; }

private:
	GameObject*  m_pGameobject;
};
