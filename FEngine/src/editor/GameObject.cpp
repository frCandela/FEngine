#include "editor/GameObject.h"
#include "editor/Transform.h"

bool GameObject::DeleteComponent(Component* component)
{
	// Find the component
	for (int i = 0; i < m_components.size(); ++i)
	{
		if (m_components[i] == component)
		{
			// Deletes it
			onComponentDeleted.emmit(this, component);
			delete(m_components[i]);
			m_components.erase(m_components.begin() + i);
			return true;
		}
	}
	return false;
}