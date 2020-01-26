#include "fanGlobalIncludes.h"
#include "editor/fanDragnDrop.h"

#include "fanImguiIcons.h"
#include "scene/fanGameobject.h"
#include "render/core/fanTexture.h"
#include "render/fanMesh.h"
#include "scene/components/fanComponent.h"
#include "scene/fanPrefab.h"

namespace ImGui
{
	//================================================================================================================================
	//================================================================================================================================
	void FanBeginDragDropSourceGameobject( fan::Gameobject * _gameobject, ImGuiDragDropFlags _flags )
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
	fan::Gameobject * FanBeginDragDropTargetGameobject()
	{
		fan::Gameobject * gameobject = nullptr;
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
	void FanBeginDragDropSourceTexture( fan::Texture * _texture, ImGuiDragDropFlags _flags )
	{
		if ( _texture != nullptr )
		{
			if ( ImGui::BeginDragDropSource( _flags ) )
			{
				ImGui::SetDragDropPayload( "dragndrop_texture", &_texture, sizeof( fan::Texture** ) );
				ImGui::Icon( ImGui::IconType::IMAGE16, { 16,16 } ); ImGui::SameLine();
				ImGui::Text( ( _texture->GetPath() ).c_str() );
				ImGui::EndDragDropSource();
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	fan::Texture * FanBeginDragDropTargetTexture()
	{
		fan::Texture * _texture = nullptr;
		if ( ImGui::BeginDragDropTarget() )
		{
			if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "dragndrop_texture" ) )
			{
				assert( payload->DataSize == sizeof( fan::Texture** ) );
				_texture = *( fan::Texture** )payload->Data;
			}
			ImGui::EndDragDropTarget();
		}
		return _texture;
	}

	//================================================================================================================================
	//================================================================================================================================
	void FanBeginDragDropSourceMesh( fan::Mesh * _mesh, ImGuiDragDropFlags _flags )
	{
		if ( _mesh != nullptr )
		{
			if ( ImGui::BeginDragDropSource( _flags ) )
			{
				ImGui::SetDragDropPayload( "dragndrop_mesh", &_mesh, sizeof( fan::Mesh** ) );
				ImGui::Icon( ImGui::IconType::MESH16, { 16,16 } ); ImGui::SameLine();
				ImGui::Text( ( _mesh->GetPath() ).c_str() );
				ImGui::EndDragDropSource();
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	fan::Mesh * FanBeginDragDropTargetMesh()
	{
		fan::Mesh * _mesh = nullptr;
		if ( ImGui::BeginDragDropTarget() )
		{
			if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "dragndrop_mesh" ) )
			{
				assert( payload->DataSize == sizeof( fan::Mesh** ) );
				_mesh = *( fan::Mesh** )payload->Data;
			}
			ImGui::EndDragDropTarget();
		}
		return _mesh;
	}

	//================================================================================================================================
//================================================================================================================================
	void FanBeginDragDropSourcePrefab( fan::Prefab * _prefab, ImGuiDragDropFlags _flags )
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
	fan::Prefab * FanBeginDragDropTargetPrefab()
	{
		fan::Prefab * _prefab = nullptr;
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
	void FanBeginDragDropSourceComponent( fan::Component * _component, ImGuiDragDropFlags _flags )
	{
		if ( _component != nullptr )
		{
			if ( ImGui::BeginDragDropSource( _flags ) )
			{
				std::string nameid = std::string( "dragndrop_" ) + _component->GetName();
				ImGui::SetDragDropPayload( nameid.c_str(), &_component, sizeof( fan::Component** ) );
				ImGui::Icon( _component->GetIcon(), { 16,16 } ); ImGui::SameLine();
				ImGui::Text( (std::string(_component->GetName()) + ":").c_str() ); ImGui::SameLine();
				ImGui::Text(  _component->GetGameobject().GetName().c_str() );
				ImGui::EndDragDropSource();
			}
		}
	}

	//================================================================================================================================
	// _typeID of the typeinfo type of the component 
	//================================================================================================================================
	fan::Component * FanBeginDragDropTargetComponent( const uint32_t _typeID )
	{
		fan::Component * component = nullptr;
		if ( ImGui::BeginDragDropTarget() )
		{
			const fan::Component * sample = fan::TypeInfo::Get().GetInstance< fan::Component >(_typeID);

			std::string nameid = std::string( "dragndrop_" ) + sample->GetName();
			// Drop payload component
			const ImGuiPayload* payloadComponent = ImGui::AcceptDragDropPayload( nameid.c_str() );
			if ( payloadComponent )
			{
				assert( payloadComponent->DataSize == sizeof( fan::Component** ) );
				component = *(fan::Component**)payloadComponent->Data;
			}
			// Drop payload gameobject
			else
			{
				const ImGuiPayload* payloadGameobject = ImGui::AcceptDragDropPayload( "dragndrop_gameobject" ) ;
				if ( payloadGameobject )
				{
					assert( payloadGameobject->DataSize == sizeof( fan::Gameobject** ) );
					fan::Gameobject * gameobject = *( fan::Gameobject** )payloadGameobject->Data;
					component = gameobject->GetComponent( _typeID );
				}
			}
			ImGui::EndDragDropTarget();
		}
		return component;
	}
}