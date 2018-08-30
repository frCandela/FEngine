#pragma once

#include "renderer/ForwardPipeline.h"
#include "editor/Component.h"

#include <array>
#include "util/KeyList.h"

// Mesh class
class Mesh : public Component
{
public:
	Mesh();

	// Loads a model from an OBJ file, returns false if loading failed, true otherwise
	bool LoadModel(std::string path);

	// Component oveload
	bool IsUnique() const override { return true; }
	std::string GetName() const override { return "Mesh"; }
	void RenderGui() override;

	// Vertices and indices of the loaded model
	std::vector<ForwardPipeline::Vertex> vertices;
	std::vector<uint32_t> indices;

	// Render id of the mesh
	key_t renderKey = nullptr;

private:
	std::string m_path;	// Obj file path
	std::array<char, 256> m_pathBuffer;	// Used to change the obj file path with imgui

};
