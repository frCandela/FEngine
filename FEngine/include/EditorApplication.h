#pragma once

#include "renderer/Renderer.h"
#include "renderer/Window.h"
#include "physics/PhysicsEngine.h"
#include "editor/Shape.h"
#include "editor/Scene.h"

class EditorApplication
{
public:
	EditorApplication();

	// Runs the application (loop)
	void Run();

	// Render Imgui parameters window
	void RenderGUI();

private:
	Renderer * renderer;
	PhysicsEngine * physicsEngine;

	Scene* scene;

	bool m_showRendererWindow = true;
	bool m_showTestWindow = false;
	bool m_showSceneHierarchy = true;
	bool m_showInspector = true;
};