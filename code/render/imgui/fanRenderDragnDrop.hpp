#pragma once

#include "imgui/imgui.h"

namespace fan
{
	class Texture;
	class Mesh;
}

//================================================================================================================================
// Imgui widgets for mesh & textures
//================================================================================================================================
namespace ImGui
{
	void			FanBeginDragDropSourceTexture( fan::Texture* _texture, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
	fan::Texture*	FanBeginDragDropTargetTexture();

	void			FanBeginDragDropSourceMesh( fan::Mesh* _mesh, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
	fan::Mesh*		FanBeginDragDropTargetMesh();
}