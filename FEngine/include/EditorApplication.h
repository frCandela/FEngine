#pragma once

#include "renderer/Renderer.h"
#include "renderer/Window.h"
#include "physics/PhysicsEngine.h"
#include "editor/Shape.h"
#include "editor/Scene.h"

#include <map>

class EditorApplication
{
public:
	EditorApplication();

	// Runs the application (loop)
	void Run();

	// Render Imgui parameters window
	void RenderGUI();

private:
	// Update the gameobject bounding box if it contains a Mesh
	void UpdateGameobjectAABB( GameObject * gameobject, bool updateGeometry = false);

	Renderer * renderer;
	PhysicsEngine * physicsEngine;

	Scene* scene;
	std::map<GameObject*, std::pair<Cube, float> > m_gameObjectsAABB; // Used for selecting objects by clicking on them (for now cubes, not AABB)

	bool m_showRendererWindow = true;
	bool m_showTestWindow = false;
	bool m_showSceneHierarchy = true;
	bool m_showInspector = true;
	bool m_editorShouldQuit = false;


};