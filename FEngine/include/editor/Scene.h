#pragma once

#include "editor/GameObject.h"
#include "imgui/imgui.h"

class Scene
{
public:
	Scene(std::string name);

	// Creates a game object and adds it to the scene hierarchy
	GameObject* CreateGameobject(std::string name);

	// Render the inspector window gui
	void RenderInspectorGui();

	// Render the scene hierarchy window and gui
	void RenderSceneGui();

private:
	std::vector<GameObject *> m_gameObjects;
	std::string m_name;
	GameObject* m_gameObjectSelected = nullptr;
};