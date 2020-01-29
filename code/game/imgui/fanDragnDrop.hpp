#pragma once

#include "game/fanGamePrecompiled.hpp"

namespace fan
{
	class Component;
	class Gameobject;
	class Texture;
	class Mesh;
	class Prefab;
}

namespace ImGui
{
	void				FanBeginDragDropSourceGameobject( fan::Gameobject * _gameobject, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
	fan::Gameobject *	FanBeginDragDropTargetGameobject();

	void				FanBeginDragDropSourceTexture( fan::Texture * _texture, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
	fan::Texture *		FanBeginDragDropTargetTexture();

	void				FanBeginDragDropSourceMesh( fan::Mesh * _mesh, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
	fan::Mesh *			FanBeginDragDropTargetMesh();

	void				FanBeginDragDropSourcePrefab( fan::Prefab * _prefab, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
	fan::Prefab *		FanBeginDragDropTargetPrefab();

	void				FanBeginDragDropSourceComponent( fan::Component * _component, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
	fan::Component *	FanBeginDragDropTargetComponent( const uint32_t _typeID );

	//================================================================================================================================
	//================================================================================================================================
	template< typename _ComponentType >
	void				FanBeginDragDropSourceComponentType( _ComponentType * _component, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None )
	{
		static_assert((std::is_base_of<fan::Component, _ComponentType>::value));
		FanBeginDragDropSourceComponent( _component, _flags );
	}

	//================================================================================================================================
	//================================================================================================================================
	template< typename _ComponentType >
	_ComponentType * FanBeginDragDropTargetComponentType()
	{
		static_assert((std::is_base_of<fan::Component, _ComponentType>::value));
		return static_cast<_ComponentType*> ( FanBeginDragDropTargetComponent( _ComponentType::s_typeID ) );
	}
}