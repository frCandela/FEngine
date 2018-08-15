#include "editor/Scene.h"
#include "editor/Transform.h"


Scene::Scene(std::string name) :
	m_name(name)
{}

GameObject* Scene::CreateGameobject(std::string name)
{
	GameObject* gameObject = new GameObject(name);
	m_gameObjects.push_back(gameObject);
	gameObject->AddComponent<Transform>();
	return gameObject;
}

void Scene::RenderInspectorGui()
{
	ImGui::Begin("Inspector");
	if (m_gameObjectSelected)
		m_gameObjectSelected->RenderGui();
	ImGui::End();
}

void Scene::RenderSceneGui()
{
	ImGui::Begin("Scene hierarchy");
	ImGui::Text(m_name.c_str());
	ImGui::Separator();
	for (int i = 0; i < m_gameObjects.size(); ++i)
	{
		GameObject* gameObject = m_gameObjects[i];

		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (gameObject == m_gameObjectSelected ? ImGuiTreeNodeFlags_Selected : 0);
		bool nodeOpen = ImGui::TreeNodeEx(gameObject->GetName().c_str(), node_flags);
		if (ImGui::IsItemClicked())
			m_gameObjectSelected = gameObject;
		if (nodeOpen)
		{
			ImGui::TreePop();
		}
	}
	ImGui::End();
}