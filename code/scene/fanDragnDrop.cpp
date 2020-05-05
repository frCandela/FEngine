#include "scene/fanDragnDrop.hpp"

#include "core/fanDebug.hpp"
#include "editor/fanImguiIcons.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/fanPrefab.hpp"
#include "scene/singletonComponents/fanScene.hpp"
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
	void FanBeginDragDropSourceComponent( fan::SceneNode& _sceneNode, fan::Component& _component, ImGuiDragDropFlags _flags )
	{
		if( ImGui::BeginDragDropSource( _flags ) )
		{
			fan::EcsWorld& world = *_sceneNode.scene->world;
			const fan::ComponentInfo& info = world.GetComponentInfo( _component.GetIndex() );

			std::string nameid = std::string( "dragndrop_" ) + std::to_string( info.staticIndex );
			ComponentPayload payload = { &_sceneNode , &_component };
			ImGui::SetDragDropPayload( nameid.c_str(), &payload, sizeof( payload ) );
			ImGui::Icon( info.icon, { 16,16 } ); ImGui::SameLine();
			ImGui::Text("%s : %s", info.name.c_str(), _sceneNode.name.c_str() );
			ImGui::EndDragDropSource();
		}		
	}

	//================================================================================================================================
	// _typeID of the typeinfo type of the component 
	//================================================================================================================================
	ComponentPayload FanBeginDragDropTargetComponent( const uint32_t _staticID )
	{
 		if( ImGui::BeginDragDropTarget() )
 		{ 
 			
			// Drop payload component
 			std::string nameid = std::string( "dragndrop_" ) + std::to_string( _staticID );
 			const ImGuiPayload* imGuiPayload = ImGui::AcceptDragDropPayload( nameid.c_str() );
 			if( imGuiPayload != nullptr )
 			{
 				assert( imGuiPayload->DataSize == sizeof( ComponentPayload ) );
				return *(ComponentPayload*)imGuiPayload->Data;
 			}

 			// Drop payload scene node
			nameid = std::string( "dragndrop_" ) + std::to_string( fan::SceneNode::s_typeInfo );
			imGuiPayload = ImGui::AcceptDragDropPayload( nameid.c_str() );
			if( imGuiPayload != nullptr )
			{
				// find the component and assigns it if it exists
				assert( imGuiPayload->DataSize == sizeof( ComponentPayload ) );
				ComponentPayload& payload = *(ComponentPayload*)imGuiPayload->Data;
				fan::SceneNode& node = *payload.sceneNode;
				fan::EcsWorld& world = *node.scene->world;
				fan::EntityID nodeID = world.GetEntityID( node.handle );
				fan::ComponentIndex componentIndex = world.GetDynamicIndex( _staticID );
				if( world.HasComponent( nodeID, componentIndex ) )
				{
					fan::Component& component = world.GetComponent( nodeID, componentIndex );
					return { &node, &component };
				}
				else
				{
					const fan::ComponentInfo& info = world.GetComponentInfo( componentIndex );
					fan::Debug::Warning() << "drop failed : " << info.name << " component not found on \"" << node.name << "\"" << fan::Debug::Endl();
				}
			}

 			ImGui::EndDragDropTarget();
 		}
 		return ComponentPayload();
	}
}