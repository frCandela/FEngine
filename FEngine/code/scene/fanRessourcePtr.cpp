#include "fanGlobalIncludes.h"
#include "scene/fanRessourcePtr.h"

#include "editor/fanDragnDrop.h"
#include "editor/fanImguiIcons.h"
#include "editor/fanModals.h"
#include "scene/fanGameobject.h"
#include "renderer/core/fanTexture.h"
#include "renderer/fanMesh.h"

namespace ImGui
{
	//================================================================================================================================
	//================================================================================================================================
	void FanGameobject( const char * _label,  fan::GameobjectPtr * _ptr )
	{
		fan::Gameobject * gameobject = **_ptr;		
		const std::string name = gameobject != nullptr ? gameobject->GetName() : "null";

		// icon
		if ( ImGui::ButtonIcon( ImGui::IconType::GAMEOBJECT16, { 16,16 } ) )
		{
			 fan::GameobjectPtr::s_onSetFromSelection.Emmit( _ptr );
		} 
		ImGui::SameLine();		

		// name button 
		float width = 0.5f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
		ImGui::Button( name.c_str(), ImVec2( width, 0.f) ); ImGui::SameLine();
			ImGui::SameLine();

		// dragndrop
		ImGui::FanBeginDragDropSourceGameobject( gameobject );
		fan::Gameobject * gameobjectDrop = ImGui::FanBeginDragDropTargetGameobject();
		if ( gameobjectDrop )				{ ( *_ptr ) =  fan::GameobjectPtr( gameobjectDrop, gameobjectDrop->GetUniqueID() ); }		
		if ( ImGui::IsItemClicked( 1 ) )	{ ( *_ptr ) =  fan::GameobjectPtr(); }	// Right click = clear

		// label	
		ImGui::Text( _label );

	}

	//================================================================================================================================
	//================================================================================================================================
	void FanTexture( const char * _label, fan::TexturePtr * _ptr )
	{		
		fan::Texture * texture = **_ptr;
		const std::string name = texture == nullptr ? "null" : std::fs::path(texture->GetPath()).filename().string() ;

		// Set button icon & modal
		const std::string modalName = std::string("Find texture (") + _label + ")";
		static std::fs::path m_pathBuffer;
		bool openModal = false;
		ImGui::PushID(_label); {			
			if ( ImGui::ButtonIcon( ImGui::IconType::IMAGE, { 16,16 } ) )
			{
				openModal = true;
			}
		} ImGui::PopID();
		if ( openModal )
		{
			ImGui::OpenPopup( modalName.c_str() );
			m_pathBuffer = "content/models";
		}
		ImGui::SameLine();
		
		// name button 
		const float width = 0.5f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
		ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ); ImGui::SameLine();
		ImGui::FanBeginDragDropSourceTexture( texture );

		// tooltip
		if( texture != nullptr ) {
			ImGui::FanToolTip( texture->GetPath().c_str() );
		}

		// dragndrop		
		fan::Texture * textureDrop = ImGui::FanBeginDragDropTargetTexture();
		if ( textureDrop ) { ( *_ptr ) = fan::TexturePtr( textureDrop, textureDrop->GetPath() ); }
		if ( ImGui::IsItemClicked( 1 ) ){( *_ptr ) = fan::TexturePtr();	}// Right click = clear
		
		if ( ImGui::FanLoadFileModal( modalName.c_str() ,  fan::GlobalValues::s_imagesExtensions, m_pathBuffer ) )
		{
			_ptr->InitUnresolved(m_pathBuffer.string());
		}

		ImGui::SameLine();
		ImGui::Text( _label );
	}

	//================================================================================================================================
	//================================================================================================================================
	void FanMesh( const char * _label, fan::MeshPtr * _ptr )
	{
		fan::Mesh * mesh = **_ptr;
		const std::string name = mesh == nullptr ? "null" : std::fs::path( mesh->GetPath() ).filename().string();

		// Set button icon & modal
		const std::string modalName = std::string( "Find mesh (" ) + _label + ")";
		static std::fs::path m_pathBuffer;
		bool openModal = false;
		ImGui::PushID( _label );
		{
			if ( ImGui::ButtonIcon( ImGui::IconType::MESH, { 16,16 } ) )
			{
				openModal = true;
			}
		} ImGui::PopID();
		if ( openModal )
		{
			ImGui::OpenPopup( modalName.c_str() );
			m_pathBuffer = "content/models";
		}
		ImGui::SameLine();

		// name button 
		const float width = 0.5f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
		ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ); ImGui::SameLine();
		ImGui::FanBeginDragDropSourceMesh( mesh );

		// tooltip
		if ( mesh != nullptr )
		{
			ImGui::FanToolTip( mesh->GetPath().c_str() );
		}

		// dragndrop		
		fan::Mesh * meshDrop = ImGui::FanBeginDragDropTargetMesh();
		if ( meshDrop ) { ( *_ptr ) = fan::MeshPtr( meshDrop, meshDrop->GetPath() ); }
		if ( ImGui::IsItemClicked( 1 ) ) { ( *_ptr ) = fan::MeshPtr(); }// Right click = clear

		if ( ImGui::FanLoadFileModal( modalName.c_str(), fan::GlobalValues::s_meshExtensions, m_pathBuffer ) )
		{
			_ptr->InitUnresolved( m_pathBuffer.string() );
		}

		ImGui::SameLine();
		ImGui::Text( _label );
	}
}
