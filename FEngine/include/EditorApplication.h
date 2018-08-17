#pragma once

#include "renderer/Renderer.h"
#include "renderer/GLFWWindow.h"
#include "physics/PhysicsEngine.h"
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
	// Updates the required modules when components are modified (AABB, selection, uniforms etc.)
	void ProcessComponentsModifications();

	void OnComponentDeletedCallback(GameObject* gameobject, Component* component);
	void OnGameobjectCreatedCallback(GameObject* gameobject);

	Renderer * m_renderer;
	PhysicsEngine * m_physicsEngine;
	Scene* m_scene;	
	EditorCamera* m_editorCamera;

	bool m_showRendererWindow = true;
	bool m_showTestWindow = false;
	bool m_showSceneHierarchy = true;
	bool m_showInspector = true;
	bool m_editorShouldQuit = false;

	std::array<char, 64> m_newGameobjectBuffer;
};