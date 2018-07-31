#pragma once

#include "renderer/Renderer.h"
#include "renderer/Window.h"
#include "physics/PhysicsEngine.h"

class FEngine
{
public:
	FEngine();

	// Runs the application (loop)
	void Run();

	// Render Imgui parameters window
	void RenderGUI();

private:
	Renderer * renderer;
	PhysicsEngine * physicsEngine;
};