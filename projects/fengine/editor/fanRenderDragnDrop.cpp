#include "editor/fanRenderDragnDrop.hpp"
#include "render/resources/fanTexture.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/resources/fanFont.hpp"
#include "editor/fanImguiIcons.hpp"

namespace ImGui
{
	//========================================================================================================
	//========================================================================================================
	void FanBeginDragDropSourceTexture( fan::Texture* _texture, ImGuiDragDropFlags _flags )
	{
		if ( _texture != nullptr )
		{
			if ( ImGui::BeginDragDropSource( _flags ) )
			{
				ImGui::SetDragDropPayload( "dragndrop_texture", &_texture, sizeof( fan::Texture** ) );
				ImGui::Icon( ImGui::IconType::Image16, { 16, 16 } ); ImGui::SameLine();
				ImGui::Text( _texture->mPath.c_str() );
				ImGui::EndDragDropSource();
			}
		}
	}

	//========================================================================================================
	//========================================================================================================
	fan::Texture* FanBeginDragDropTargetTexture()
	{
		fan::Texture* texture = nullptr;
		if ( ImGui::BeginDragDropTarget() )
		{
			if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "dragndrop_texture" ) )
			{
                fanAssert( payload->DataSize == sizeof( fan::Texture** ) );
                texture = *( fan::Texture** )payload->Data;
			}
			ImGui::EndDragDropTarget();
		}
		return texture;
	}

	//========================================================================================================
	//========================================================================================================
	void FanBeginDragDropSourceMesh( fan::Mesh* _mesh, ImGuiDragDropFlags _flags )
	{
		if ( _mesh != nullptr )
		{
			if ( ImGui::BeginDragDropSource( _flags ) )
			{
				ImGui::SetDragDropPayload( "dragndrop_mesh", &_mesh, sizeof( fan::Mesh** ) );
				ImGui::Icon( ImGui::IconType::Mesh16, { 16, 16 } ); ImGui::SameLine();
				ImGui::Text( _mesh->mPath.c_str() );
				ImGui::EndDragDropSource();
			}
		}
	}

	//========================================================================================================
	//========================================================================================================
	fan::Mesh* FanBeginDragDropTargetMesh()
	{
		fan::Mesh* mesh = nullptr;
		if ( ImGui::BeginDragDropTarget() )
		{
			if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "dragndrop_mesh" ) )
			{
                fanAssert( payload->DataSize == sizeof( fan::Mesh** ) );
                mesh = *( fan::Mesh** )payload->Data;
			}
			ImGui::EndDragDropTarget();
		}
		return mesh;
	}

    //========================================================================================================
    //========================================================================================================
    void FanBeginDragDropSourceFont( fan::Font* _font, ImGuiDragDropFlags _flags )
    {
        if ( _font != nullptr )
        {
            if ( ImGui::BeginDragDropSource( _flags ) )
            {
                ImGui::SetDragDropPayload( "dragndrop_font", &_font, sizeof( fan::Font** ) );
                ImGui::Icon( ImGui::IconType::Font16, { 16, 16 } ); ImGui::SameLine();
                ImGui::Text( _font->GetPath().c_str() );
                ImGui::EndDragDropSource();
            }
        }
    }

    //========================================================================================================
    //========================================================================================================
    fan::Font* FanBeginDragDropTargetFont()
    {
        fan::Font* font = nullptr;
        if ( ImGui::BeginDragDropTarget() )
        {
            if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "dragndrop_font" ) )
            {
                fanAssert( payload->DataSize == sizeof( fan::Font** ) );
                font = *( fan::Font** )payload->Data;
            }
            ImGui::EndDragDropTarget();
        }
        return font;
    }
}