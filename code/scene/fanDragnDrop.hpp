#pragma once

#include <cstdint>
#include "imgui/imgui.h"
#include "ecs/fanEcsTypes.hpp"

namespace fan
{
	class  Prefab;
	struct EcsComponentInfo;
	class EcsWorld;
}

namespace ImGui
{
	//================================================================================================================================
	// helper struct for component drag & drop
	//================================================================================================================================
	struct ComponentPayload
	{
	    static const std::string sPrefix;
		fan::EcsHandle           mHandle        = 0;
		uint32_t                 mComponentType = 0;

		static bool IsComponentPayload( const ImGuiPayload * _payload );
	};

	void				FanBeginDragDropSourcePrefab( fan::Prefab * _prefab, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
	fan::Prefab *		FanBeginDragDropTargetPrefab();

	void				FanBeginDragDropSourceComponent( fan::EcsWorld& _world, fan::EcsHandle& _handle, uint32_t _type, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
	ComponentPayload	FanBeginDragDropTargetComponent( fan::EcsWorld& _world, uint32_t _type );


 	//================================================================================================================================
 	//================================================================================================================================
 	template< typename _componentType >
	ComponentPayload FanBeginDragDropTargetComponent( fan::EcsWorld& _world )
 	{
 		static_assert((std::is_base_of<fan::EcsComponent, _componentType>::value));
 		return FanBeginDragDropTargetComponent( _world, _componentType::Info::s_type );
 	}
}