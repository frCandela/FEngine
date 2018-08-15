#pragma once

#include "imgui/imgui.h"


#include <string>

#include "editor/GameObject.h"
class GameObject;

class Component
{
public:
	friend class GameObject;

	// Returns true is the component is Unique (only one instance per GameObject) false otherwise
	virtual bool IsUnique() const = 0;

	// Returns the name of the gameObject
	virtual std::string GetName() const = 0;

	// Returns true if the component was modified since the last "IsModified()" call. (look up the m_wasModified variable)
	bool WasModified();

	// Render the inspector gui of the game object
	virtual void RenderGui() { ImGui::Text(GetName().c_str()); }

	// Returns a reference on the Gameobject the component is bound to
	inline GameObject* GetGameobject() const { return m_pGameobject; }

	Signal<> onComponentDelete;

protected:
	bool m_wasModified = true;	// Set this variable to true if the component needs to be updated by the others systems using it.

private:
	GameObject*  m_pGameobject;	
};
