#include "fanGlobalIncludes.h"
#include "editor/fanDragnDrop.h"

#include "fanImguiIcons.h"
#include "scene/fanGameobject.h"
#include "renderer/core/fanTexture.h"

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
				ImGui::Icon( ImGui::IconType::IMAGE, { 16,16 } ); ImGui::SameLine();
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
}