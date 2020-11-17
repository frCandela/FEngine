#pragma once

#include "imgui/imgui.h"

namespace fan
{
	struct Texture;
	struct Mesh;
    class Font;
}

//===========================================================================================================
// Imgui widgets for mesh & textures
//===========================================================================================================
namespace ImGui
{
    void FanBeginDragDropSourceTexture( fan::Texture* _texture,
                                        ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
    fan::Texture* FanBeginDragDropTargetTexture();

    void FanBeginDragDropSourceMesh( fan::Mesh* _mesh,
                                     ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
	fan::Mesh*		FanBeginDragDropTargetMesh();

    void FanBeginDragDropSourceFont( fan::Font* _font,
                                     ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
    fan::Font*		FanBeginDragDropTargetFont();
}