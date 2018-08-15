#pragma once

#include "editor/GameObject.h"
#include "editor/Shape.h"
#include "imgui/imgui.h"
#include "renderer/Renderer.h"
#include <map>

// A Scene is a tree of gameobjects
class Scene
{
public:
	// Constructor
	Scene(std::string name);

	// Creates a game object and adds it to the scene hierarchy
	GameObject* CreateGameobject(std::string name);

	// Deletes a gameobject and removes it from the scene hierarchy
	void DeleteGameobjectLater(GameObject* gameobject);

	// Returns a vector referencing the "to delete" gameobjects 
	std::vector<GameObject *> GetGameobjectsToDelete() const;

	// Apply DeleteGameobjectLater commands  
	void ApplyDeleteCommands();

	// Render the inspector window gui
	void RenderInspectorGui();

	// Render the scene hierarchy window and gui
	void RenderSceneGui();

	std::map<GameObject*, GameObject *> const & GetGameObjects() { return m_gameObjects; }

	// Returns the selected Gameobject
	GameObject* GetSelectedGameobject();

	// Sets the selected Gameobject
	void SetSelectedGameobject( GameObject * newSelectedGameobject);

	// Draws the AABB of the selected gameobject
	void DrawSelectionAABB(Renderer& renderer);

	// Update the gameobject bounding box if it contains a Mesh
	void UpdateGameobjectAABB(GameObject * gameobject, bool updateGeometry = false);

	// Raycast on the scene against every gameobject AABB
	GameObject* RayCast(Ray ray);
private:
	std::vector <GameObject*> m_toDeleteLater;
	std::map<GameObject *, GameObject *> m_gameObjects;
	std::string m_name;
	GameObject* m_gameObjectSelected = nullptr;

	std::map<GameObject*, std::pair<Cube, float> > m_gameObjectsAABB; // Used for selecting objects by clicking on them (for now cubes, not AABB)
};