#include "GameObject.h"

bool GameObject::DeleteComponent(Component* component)
{
	for (int i = 0; i < m_components.size(); ++i)
	{
		if (m_components[i] == component)
		{
			delete(m_components[i]);
			m_components.erase(m_components.begin() + i);
			return true;
		}
	}
	return false;
}
