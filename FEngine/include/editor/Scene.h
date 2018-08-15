#pragma once

#include "editor/GameObject.h"
#include "imgui/imgui.h"

// A Scene is a tree of gameobjects
class Scene
{
public:
	// Constructor
	Scene(std::string name);

	// Creates a game object and adds it to the scene hierarchy
	GameObject* CreateGameobject(std::string name);

	// Render the inspector window gui
	void RenderInspectorGui();

	// Render the scene hierarchy window and gui
	void RenderSceneGui();

	std::vector<GameObject*> const & GetGameObjects() { return m_gameObjects; }

	// Returns the selected Gameobject
	GameObject* GetSelectedGameobject();

	// Sets the selected Gameobject
	void SetSelectedGameobject( GameObject * newSelectedGameobject);

private:
	std::vector<GameObject *> m_gameObjects;
	std::string m_name;
	GameObject* m_gameObjectSelected = nullptr;


};