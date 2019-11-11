#pragma once

namespace fan
{
	class Gameobject;
	class Texture;
}

namespace ImGui
{
	void				BeginDragDropSourceGameobject( fan::Gameobject * _gameobject, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
	fan::Gameobject *	BeginDragDropTargetGameobject();

	void			BeginDragDropSourceTexture( fan::Texture * _texture, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
	fan::Texture *	BeginDragDropTargetTexture();
}