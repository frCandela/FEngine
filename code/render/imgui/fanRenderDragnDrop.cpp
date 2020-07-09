#include "render/imgui/fanRenderDragnDrop.hpp"
#include "render/core/fanTexture.hpp"
#include "render/fanMesh.hpp"
#include "editor/fanImguiIcons.hpp"

namespace ImGui
{
	//================================================================================================================================
	//================================================================================================================================
	void FanBeginDragDropSourceTexture( fan::Texture* _texture, ImGuiDragDropFlags _flags )
	{
		if ( _texture != nullptr )
		{
			if ( ImGui::BeginDragDropSource( _flags ) )
			{
				ImGui::SetDragDropPayload( "dragndrop_texture", &_texture, sizeof( fan::Texture** ) );
				ImGui::Icon( ImGui::IconType::IMAGE16, { 16,16 } ); ImGui::SameLine();
				ImGui::Text( _texture->mPath.c_str() );
				ImGui::EndDragDropSource();
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	fan::Texture* FanBeginDragDropTargetTexture()
	{
		fan::Texture* _texture = nullptr;
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
	void FanBeginDragDropSourceMesh( fan::Mesh* _mesh, ImGuiDragDropFlags _flags )
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
	fan::Mesh* FanBeginDragDropTargetMesh()
	{
		fan::Mesh* _mesh = nullptr;
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
}