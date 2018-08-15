#pragma once

#include "renderer/Renderer.h"
#include "renderer/Window.h"
#include "physics/PhysicsEngine.h"
#include "editor/Shape.h"
#include "editor/Scene.h"

#include "editor/EditorCamera.h"

#include <map>

class EditorApplication
{
public:
	// Runs the application (loop)
	void Run();

	// Render Imgui parameters window
	void RenderGUI();

private:

	// Update the gameobject bounding box if it contains a Mesh
	void UpdateGameobjectAABB( GameObject * gameobject, bool updateGeometry = false);

	// Updates the required modules when components are modified (AABB, selection, uniforms etc.)
	void ProcessComponentsModifications();

	// Raycast where the user clicked to select a gameobject
	void SelectGameobject();

	// Draws the AABB of the selected gameobject
	void DrawSelectionAABB();

	Renderer * m_renderer;
	PhysicsEngine * m_physicsEngine;
	Scene* m_scene;
	std::map<GameObject*, std::pair<Cube, float> > m_gameObjectsAABB; // Used for selecting objects by clicking on them (for now cubes, not AABB)
	EditorCamera* m_editorCamera;

	bool m_showRendererWindow = true;
	bool m_showTestWindow = false;
	bool m_showSceneHierarchy = true;
	bool m_showInspector = true;
	bool m_editorShouldQuit = false;
};