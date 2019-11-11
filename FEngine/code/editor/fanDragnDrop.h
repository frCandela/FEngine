#pragma once

namespace fan
{
	class Gameobject;
	class Texture;
}

namespace ImGui
{
	void				FanBeginDragDropSourceGameobject( fan::Gameobject * _gameobject, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
	fan::Gameobject *	FanBeginDragDropTargetGameobject();

	void				FanBeginDragDropSourceTexture( fan::Texture * _texture, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
	fan::Texture *		FanBeginDragDropTargetTexture();
}