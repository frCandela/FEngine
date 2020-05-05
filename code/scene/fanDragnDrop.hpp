#pragma once

#include "fanImgui.hpp"
#include <cstdint>

namespace fan
{
	class Prefab;
	struct SceneNode;
	struct Component;
	struct ComponentInfo;
}

namespace ImGui
{
	//================================================================================================================================
	// helper struct for component drag & drop
	//================================================================================================================================
	struct ComponentPayload
	{
		fan::SceneNode* sceneNode = nullptr;
		fan::Component* component = nullptr;
	};

	void				FanBeginDragDropSourcePrefab( fan::Prefab * _prefab, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
	fan::Prefab *		FanBeginDragDropTargetPrefab();

	void				FanBeginDragDropSourceComponent( fan::SceneNode& _sceneNode, fan::Component& _component, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
	ComponentPayload	FanBeginDragDropTargetComponent( const uint32_t _staticID );


 	//================================================================================================================================
 	//================================================================================================================================
 	template< typename _componentType >
	ComponentPayload FanBeginDragDropTargetComponent()
 	{
 		static_assert((std::is_base_of<fan::Component, _componentType>::value));
 		return FanBeginDragDropTargetComponent( _componentType::s_typeInfo );
 	}
}