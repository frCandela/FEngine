#include "render/fanRenderResourcePtr.hpp"

#include <sstream>
#include "render/fanRenderDragnDrop.hpp"
#include "editor/fanModals.hpp"

namespace ImGui
{

	//========================================================================================================
	// Draws a ImGui widget for displaying a TexturePtr
	// Returns true if the value (TexturePtr) was edited
	//========================================================================================================
	bool FanTexturePtr( const char* _label, fan::TexturePtr& _ptr )
	{
		bool returnValue = false;

		fan::Texture* texture = *_ptr;
        const std::string name = texture == nullptr ?
                                 "null" :
                                 std::filesystem::path( texture->mPath ).filename().string();

		// Set button icon & modal
		const std::string modalName = std::string( "Find texture (" ) + _label + ")";
		static std::filesystem::path m_pathBuffer;
		bool openModal = false;
		ImGui::PushID( _label );
		{
			if ( ImGui::ButtonIcon( ImGui::IconType::Image16, { 16, 16 } ) )
			{
				openModal = true;
			}
		} ImGui::PopID();
		if ( openModal )
		{
			ImGui::OpenPopup( modalName.c_str() );
			if( m_pathBuffer.empty() )
            {
                m_pathBuffer = fan::RenderGlobal::sContentPath;
            }
		}
		ImGui::SameLine();

		// name button 
		const float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 23;
		ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ); ImGui::SameLine();
		ImGui::FanBeginDragDropSourceTexture( texture );

		// tooltip
		if ( texture != nullptr )
		{
			if ( ImGui::IsItemHovered() )
			{
				ImGui::BeginTooltip();

				// path
				ImGui::Text( texture->mPath.c_str() );

				// size
				std::stringstream ss;
				ss << texture->mExtent.width << " x " << texture->mExtent.height
				   << " x " << texture->mLayerCount;
				ImGui::Text( ss.str().c_str() );

				ImGui::EndTooltip();
			}
		}

		// dragndrop		
		fan::Texture* textureDrop = ImGui::FanBeginDragDropTargetTexture();
		if ( textureDrop )
		{
			_ptr = textureDrop ;
			returnValue = true;
		}

		// Right click = clear
		if ( ImGui::IsItemClicked( 1 ) )
		{
			_ptr = nullptr;
			returnValue = true;
		}

		// Modal set value
        if( ImGui::FanLoadFileModal( modalName.c_str(),
                                     fan::RenderGlobal::sImagesExtensions, m_pathBuffer ) )
        {
			_ptr.Init( m_pathBuffer.string() );
			_ptr.Resolve();
			returnValue = true;
		}

		ImGui::SameLine();
		ImGui::Text( _label );

		return returnValue;
	}

	//========================================================================================================
	// Draws a ImGui widget for displaying a MeshPtr
	// Returns true if the value (MeshPtr) was edited
	//========================================================================================================
	bool FanMeshPtr( const char* _label, fan::MeshPtr& _ptr )
	{
		bool returnValue = false;

		fan::Mesh* mesh = *_ptr;
        const std::string name = ( mesh == nullptr ) ?
                "null" :
                std::filesystem::path( mesh->mPath ).filename().string();

		// Set button icon & modal
		const std::string modalName = std::string( "Find mesh (" ) + _label + ")";
		static std::filesystem::path m_pathBuffer;
		bool openModal = false;
		ImGui::PushID( _label );
		{
			if ( ImGui::ButtonIcon( ImGui::IconType::Mesh16, { 16, 16 } ) )
			{
				openModal = true;
			}
		}
		ImGui::PopID();
		if ( openModal )
		{
			ImGui::OpenPopup( modalName.c_str() );
			if( m_pathBuffer.empty() )
            {
                m_pathBuffer = fan::RenderGlobal::sContentPath;
            }
		}
		ImGui::SameLine();

		// name button 
		const float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
		ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ); ImGui::SameLine();
		ImGui::FanBeginDragDropSourceMesh( mesh );

		// tooltip
		if ( mesh != nullptr )
		{
			ImGui::FanToolTip( mesh->mPath.c_str() );
		}

		// dragndrop		
		fan::Mesh* meshDrop = ImGui::FanBeginDragDropTargetMesh();
		if ( meshDrop )
		{
			_ptr = meshDrop;
			returnValue = true;
		}

		// Right click = clear
		if ( ImGui::IsItemClicked( 1 ) )
		{
			_ptr = nullptr;
			returnValue = true;
		}

		if ( ImGui::FanLoadFileModal( modalName.c_str(), fan::RenderGlobal::sMeshExtensions, m_pathBuffer ) )
		{
			_ptr.Init( m_pathBuffer.string() );
			_ptr.Resolve();
			returnValue = true;
		}

		ImGui::SameLine();
		ImGui::Text( _label );

		return returnValue;
	}
}
