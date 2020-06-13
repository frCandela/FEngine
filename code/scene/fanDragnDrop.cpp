#include "scene/fanDragnDrop.hpp"

#include "core/fanDebug.hpp"
#include "editor/fanImguiIcons.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/fanPrefab.hpp"
#include "scene/singletons/fanScene.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "render/core/fanTexture.hpp"
#include "render/fanMesh.hpp"

namespace ImGui
{
	//================================================================================================================================
	//================================================================================================================================
	void FanBeginDragDropSourcePrefab( fan::Prefab* _prefab, ImGuiDragDropFlags _flags )
	{
		if ( _prefab != nullptr )
		{
			if ( ImGui::BeginDragDropSource( _flags ) )
			{
				ImGui::SetDragDropPayload( "dragndrop_prefab", &_prefab, sizeof( fan::Prefab** ) );
				ImGui::Icon( ImGui::IconType::PREFAB16, { 16,16 } ); ImGui::SameLine();
				ImGui::Text( ( _prefab->GetPath() ).c_str() );
				ImGui::EndDragDropSource();
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	fan::Prefab* FanBeginDragDropTargetPrefab()
	{
		fan::Prefab* _prefab = nullptr;
		if ( ImGui::BeginDragDropTarget() )
		{
			if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "dragndrop_prefab" ) )
			{
				assert( payload->DataSize == sizeof( fan::Prefab** ) );
				_prefab = *( fan::Prefab** )payload->Data;
			}
			ImGui::EndDragDropTarget();
		}
		return _prefab;
	}

	//================================================================================================================================
	//================================================================================================================================
	void FanBeginDragDropSourceComponent( fan::EcsWorld& _world, fan::EcsHandle& _handle, uint32_t _type, ImGuiDragDropFlags _flags )
	{
		if( ImGui::BeginDragDropSource( _flags ) )
		{
			const fan::EcsComponentInfo& info = _world.GetComponentInfo( _type );

			std::string nameid = std::string( "dragndrop_" ) + std::to_string( info.type);
			ComponentPayload payload = { _handle , _type };
			ImGui::SetDragDropPayload( nameid.c_str(), &payload, sizeof( payload ) );
			ImGui::Icon( info.icon, { 16,16 } ); ImGui::SameLine();
			ImGui::Text("%s : %s", info.name.c_str(), info.name.c_str() );
			ImGui::EndDragDropSource();
		}		
	}

	//================================================================================================================================
	//================================================================================================================================
	ComponentPayload FanBeginDragDropTargetComponent( fan::EcsWorld& _world, uint32_t _type )
	{
 		if( ImGui::BeginDragDropTarget() )
 		{  			
			// Drop payload component
 			std::string nameid = std::string( "dragndrop_" ) + std::to_string( _type );
 			const ImGuiPayload* imGuiPayload = ImGui::AcceptDragDropPayload( nameid.c_str() );
 			if( imGuiPayload != nullptr )
 			{
 				assert( imGuiPayload->DataSize == sizeof( ComponentPayload ) );
				return *(ComponentPayload*)imGuiPayload->Data;
 			}

 			// Drop payload scene node
			nameid = std::string( "dragndrop_" ) + std::to_string( fan::SceneNode::Info::s_type );
			imGuiPayload = ImGui::AcceptDragDropPayload( nameid.c_str() );
			if( imGuiPayload != nullptr )
			{
				// find the component and assigns it if it exists
				assert( imGuiPayload->DataSize == sizeof( ComponentPayload ) );
				ComponentPayload& payload = *(ComponentPayload*)imGuiPayload->Data;
				if( _world.HasComponent( _world.GetEntity( payload.handle ), _type ) )
				{
					return payload;
				}
				else
				{
					const fan::EcsComponentInfo& info = _world.GetComponentInfo( _type );
					fan::Debug::Warning() << "dropped scene node doesn't have a " << info.name << " component" << fan::Debug::Endl();
				}
			}

 			ImGui::EndDragDropTarget();
 		}
 		return ComponentPayload();
	}
}