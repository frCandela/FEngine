#pragma once

#include "renderer/ForwardPipeline.h"
#include "editor/Component.h"

// Mesh class
class Mesh : public Component
{
public:
	// Loads a model from an OBJ file
	void LoadModel(std::string path);

	// Component oveload
	bool IsUnique() const override { return true; }
	std::string GetName() const override { return "Mesh"; }
	void RenderGui() override;

	// Vertices and indices of the loaded model
	std::vector<ForwardPipeline::Vertex> vertices;
	std::vector<uint32_t> indices;

	// Render id of the mesh
	render_id renderId = nullptr;

};
