#pragma once

#include "renderer/Renderer.h"
#include "renderer/Window.h"

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
};