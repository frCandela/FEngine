#pragma once

#include "GameObject.h"
#include "imgui/imgui.h"

class GameObject;

class Component
{
public:
	friend class GameObject;

	// Returns true is the component is Unique (only one instance per GameObject) false otherwise
	virtual bool IsUnique() const = 0;

	// Returns the name of the gameObject
	virtual std::string GetName() const = 0;

	// Render the inspector gui of the game object
	virtual void RenderGui() { ImGui::Text(GetName().c_str()); }

	// Returns a reference on the Gameobject the component is bound to
	 inline GameObject* GetGameobject() const { return m_pGameobject; }

private:
	GameObject*  m_pGameobject;	
};
