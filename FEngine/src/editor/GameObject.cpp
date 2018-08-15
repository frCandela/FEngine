#include "editor/GameObject.h"
#include "editor/Transform.h"

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

void GameObject::RenderGui()
{
	ImGui::Text("GameObject : %s", m_name.c_str());
	for (Component* component : m_components)
	{
		ImGui::Separator();
		component->RenderGui();
	}
	ImGui::Separator();
}