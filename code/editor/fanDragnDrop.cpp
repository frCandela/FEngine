#include "editor/fanDragnDrop.hpp"

#include "editor/fanImguiIcons.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/fanPrefab.hpp"
#include "render/core/fanTexture.hpp"
#include "render/fanMesh.hpp"

namespace ImGui
{
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void FanBeginDragDropSourceGameobject( fan::Gameobject* _gameobject, ImGuiDragDropFlags _flags )
// 	{
// 		if ( _gameobject != nullptr )
// 		{
// 			if ( ImGui::BeginDragDropSource( _flags ) )
// 			{
// 				ImGui::SetDragDropPayload( "dragndrop_gameobject", &_gameobject, sizeof( fan::Gameobject** ) );
// 				ImGui::Icon( ImGui::IconType::GAMEOBJECT16, { 16,16 } ); ImGui::SameLine();
// 				ImGui::Text( ( _gameobject->GetName() ).c_str() );
// 				ImGui::EndDragDropSource();
// 			}
// 		}
// 	}

// 	//================================================================================================================================
// 	//================================================================================================================================
// 	fan::Gameobject* FanBeginDragDropTargetGameobject()
// 	{
// 		fan::Gameobject* gameobject = nullptr;
// 		if ( ImGui::BeginDragDropTarget() )
// 		{
// 			if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "dragndrop_gameobject" ) )
// 			{
// 				assert( payload->DataSize == sizeof( fan::Gameobject** ) );
// 				gameobject = *( fan::Gameobject** )payload->Data;
// 			}
// 			ImGui::EndDragDropTarget();
// 		}
// 		return gameobject;
// 	}

	//================================================================================================================================
	//================================================================================================================================
	void FanBeginDragDropSourceSceneNode( fan::SceneNode& _node, ImGuiDragDropFlags _flags  )
	{
		fan::SceneNode * node = &_node;

		if( ImGui::BeginDragDropSource( _flags ) )
		{
			ImGui::SetDragDropPayload( "dragndrop_scenenode", &node, sizeof( fan::SceneNode** ) );
			ImGui::Icon( ImGui::IconType::GAMEOBJECT16, { 16,16 } ); ImGui::SameLine();
			ImGui::Text( node->name.c_str() );
			ImGui::EndDragDropSource();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	fan::SceneNode* FanBeginDragDropTargetSceneNode()
	{
		fan::SceneNode* node = nullptr;
		if( ImGui::BeginDragDropTarget() )
		{
			if( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "dragndrop_scenenode" ) )
			{
				assert( payload->DataSize == sizeof( fan::SceneNode** ) );
				node = *( fan::SceneNode** )payload->Data;
			}
			ImGui::EndDragDropTarget();
		}
		return node;
	}

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

// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void FanBeginDragDropSourceComponent( fan::Component* _component, ImGuiDragDropFlags _flags )
// 	{
// 		if ( _component != nullptr )
// 		{
// 			if ( ImGui::BeginDragDropSource( _flags ) )
// 			{
// 				std::string nameid = std::string( "dragndrop_" ) + _component->GetName();
// 				ImGui::SetDragDropPayload( nameid.c_str(), &_component, sizeof( fan::Component** ) );
// 				ImGui::Icon( _component->GetIcon(), { 16,16 } ); ImGui::SameLine();
// 				ImGui::Text( ( std::string( _component->GetName() ) + ":" ).c_str() ); ImGui::SameLine();
// 				ImGui::Text( _component->GetGameobject().GetName().c_str() );
// 				ImGui::EndDragDropSource();
// 			}
// 		}
// 	}
// 
// 	//================================================================================================================================
// 	// _typeID of the typeinfo type of the component 
// 	//================================================================================================================================
// 	fan::Component* FanBeginDragDropTargetComponent( const uint32_t _typeID )
// 	{
// 		fan::Component* component = nullptr;
// 		if ( ImGui::BeginDragDropTarget() )
// 		{
// 			const fan::Component* sample = fan::TypeInfo::Get().GetInstance< fan::Component >( _typeID );
// 
// 			std::string nameid = std::string( "dragndrop_" ) + sample->GetName();
// 			// Drop payload component
// 			const ImGuiPayload* payloadComponent = ImGui::AcceptDragDropPayload( nameid.c_str() );
// 			if ( payloadComponent )
// 			{
// 				assert( payloadComponent->DataSize == sizeof( fan::Component** ) );
// 				component = *( fan::Component** )payloadComponent->Data;
// 			}
// 			// Drop payload gameobject
// 			else
// 			{
// 				const ImGuiPayload* payloadGameobject = ImGui::AcceptDragDropPayload( "dragndrop_gameobject" );
// 				if ( payloadGameobject )
// 				{
// 					assert( payloadGameobject->DataSize == sizeof( fan::Gameobject** ) );
// 					fan::Gameobject* gameobject = *( fan::Gameobject** )payloadGameobject->Data;
// 					component = gameobject->GetComponent( _typeID );
// 				}
// 			}
// 			ImGui::EndDragDropTarget();
// 		}
// 		return component;
// 	}

	//================================================================================================================================
	//================================================================================================================================
	void FanBeginDragDropSourceEcComponent( fan::Component& _component, const fan::ComponentInfo& _info, ImGuiDragDropFlags _flags )
	{
		fan::Component* component = &_component;
		if( ImGui::BeginDragDropSource( _flags ) )
		{
			std::string nameid = std::string( "dragndrop_" ) + std::to_string( _component.GetIndex() );
			ImGui::SetDragDropPayload( nameid.c_str(), &component, sizeof( fan::Component** ) );
			ImGui::Icon( _info.icon, { 16,16 } ); ImGui::SameLine();
			ImGui::Text( _info.name.c_str() );
			ImGui::EndDragDropSource();
		}		
	}

	//================================================================================================================================
	// _typeID of the typeinfo type of the component 
	//================================================================================================================================
	fan::Component* FanBeginDragDropTargetEcComponent( const uint32_t _typeID )
	{
 		fan::Component* component = nullptr;
// 		if( ImGui::BeginDragDropTarget() )
// 		{
// 			const fan::Component* sample = fan::TypeInfo::Get().GetInstance< fan::Component >( _typeID );
// 
// 			std::string nameid = std::string( "dragndrop_" ) + sample->GetName();
// 			// Drop payload component
// 			const ImGuiPayload* payloadComponent = ImGui::AcceptDragDropPayload( nameid.c_str() );
// 			if( payloadComponent )
// 			{
// 				assert( payloadComponent->DataSize == sizeof( fan::Component** ) );
// 				component = *( fan::Component** )payloadComponent->Data;
// 			}
// 			// Drop payload gameobject
// 			else
// 			{
// 				const ImGuiPayload* payloadGameobject = ImGui::AcceptDragDropPayload( "dragndrop_gameobject" );
// 				if( payloadGameobject )
// 				{
// 					assert( payloadGameobject->DataSize == sizeof( fan::Gameobject** ) );
// 					fan::Gameobject* gameobject = *( fan::Gameobject** )payloadGameobject->Data;
// 					component = gameobject->GetComponent( _typeID );
// 				}
// 			}
// 			ImGui::EndDragDropTarget();
// 		}
 		return component;
 	}
}