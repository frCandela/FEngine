#include "game/imgui/fanDragnDrop.hpp"
#include "core/imgui/fanImguiIcons.hpp"
#include "scene/components/fanComponent.hpp"
#include "scene/fanGameobject.hpp"
#include "scene/fanPrefab.hpp"
#include "render/core/fanTexture.hpp"
#include "render/fanMesh.hpp"

namespace ImGui
{
	//================================================================================================================================
	//================================================================================================================================
	void FanBeginDragDropSourceGameobject( fan::Gameobject* _gameobject, ImGuiDragDropFlags _flags )
	{
		if ( _gameobject != nullptr )
		{
			if ( ImGui::BeginDragDropSource( _flags ) )
			{
				ImGui::SetDragDropPayload( "dragndrop_gameobject", &_gameobject, sizeof( fan::Gameobject** ) );
				ImGui::Icon( ImGui::IconType::GAMEOBJECT16, { 16,16 } ); ImGui::SameLine();
				ImGui::Text( ( _gameobject->GetName() ).c_str() );
				ImGui::EndDragDropSource();
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	fan::Gameobject* FanBeginDragDropTargetGameobject()
	{
		fan::Gameobject* gameobject = nullptr;
		if ( ImGui::BeginDragDropTarget() )
		{
			if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "dragndrop_gameobject" ) )
			{
				assert( payload->DataSize == sizeof( fan::Gameobject** ) );
				gameobject = *( fan::Gameobject** )payload->Data;
			}
			ImGui::EndDragDropTarget();
		}
		return gameobject;
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

	//================================================================================================================================
	//================================================================================================================================
	void FanBeginDragDropSourceComponent( fan::Component* _component, ImGuiDragDropFlags _flags )
	{
		if ( _component != nullptr )
		{
			if ( ImGui::BeginDragDropSource( _flags ) )
			{
				std::string nameid = std::string( "dragndrop_" ) + _component->GetName();
				ImGui::SetDragDropPayload( nameid.c_str(), &_component, sizeof( fan::Component** ) );
				ImGui::Icon( _component->GetIcon(), { 16,16 } ); ImGui::SameLine();
				ImGui::Text( ( std::string( _component->GetName() ) + ":" ).c_str() ); ImGui::SameLine();
				ImGui::Text( _component->GetGameobject().GetName().c_str() );
				ImGui::EndDragDropSource();
			}
		}
	}

	//================================================================================================================================
	// _typeID of the typeinfo type of the component 
	//================================================================================================================================
	fan::Component* FanBeginDragDropTargetComponent( const uint32_t _typeID )
	{
		fan::Component* component = nullptr;
		if ( ImGui::BeginDragDropTarget() )
		{
			const fan::Component* sample = fan::TypeInfo::Get().GetInstance< fan::Component >( _typeID );

			std::string nameid = std::string( "dragndrop_" ) + sample->GetName();
			// Drop payload component
			const ImGuiPayload* payloadComponent = ImGui::AcceptDragDropPayload( nameid.c_str() );
			if ( payloadComponent )
			{
				assert( payloadComponent->DataSize == sizeof( fan::Component** ) );
				component = *( fan::Component** )payloadComponent->Data;
			}
			// Drop payload gameobject
			else
			{
				const ImGuiPayload* payloadGameobject = ImGui::AcceptDragDropPayload( "dragndrop_gameobject" );
				if ( payloadGameobject )
				{
					assert( payloadGameobject->DataSize == sizeof( fan::Gameobject** ) );
					fan::Gameobject* gameobject = *( fan::Gameobject** )payloadGameobject->Data;
					component = gameobject->GetComponent( _typeID );
				}
			}
			ImGui::EndDragDropTarget();
		}
		return component;
	}
}