#pragma once

#include "renderer/ForwardPipeline.h"
#include "editor/Component.h"

#include <array>

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
	render_id renderId = nullptr;

	bool NeedsUpdate() const { return m_pathChanged; } // Return true if the mesh was changed and needs an update
	void SetUpdated() { m_pathChanged = false; }



private:
	std::string m_path;
	std::array<char, 256> m_pathBuffer;
	bool m_pathChanged = false;

};
