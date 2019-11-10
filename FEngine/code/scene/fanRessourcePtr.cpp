#include "fanGlobalIncludes.h"
#include "scene/fanRessourcePtr.h"

#include "scene/fanGameobject.h"
#include "renderer/core/fanTexture.h"
#include "editor/fanImguiIcons.h"
#include "editor/fanModals.h"

namespace ImGui
{
	//================================================================================================================================
	//================================================================================================================================
	void InputGameobject( const char * _label,  fan::GameobjectPtr * _ptr )
	{
		fan::Gameobject * gameobject = **_ptr;		

		std::string name = gameobject != nullptr ? gameobject->GetName() : "null";


		// Set from selection icon
		if ( ImGui::ButtonIcon( ImGui::IconType::GAMEOBJECT16, { 16,16 } ) )
		{
			 fan::GameobjectPtr::s_onSetFromSelection.Emmit( _ptr );
		}

		ImGui::SameLine();
		float width = 0.5f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;



		ImGui::Button( name.c_str(), ImVec2( width, 0.f) ); ImGui::SameLine();
		// dragndrop
		ImGui::BeginDragDropSourceGameobject( gameobject );
		fan::Gameobject * gameobjectDrop = ImGui::BeginDragDropTargetGameobject();
		if ( gameobjectDrop ) { ( *_ptr ) =  fan::GameobjectPtr( gameobjectDrop, gameobjectDrop->GetUniqueID() ); }
		
		if ( ImGui::IsItemClicked( 1 ) )
		{
			(*_ptr ) =  fan::GameobjectPtr();
		}


		ImGui::SameLine();
		ImGui::Text( _label );

	}

	//================================================================================================================================
	//================================================================================================================================
	void InputTexture( const char * _label, fan::TexturePtr * _ptr )
	{
		fan::Texture * texture = **_ptr;

		std::string name = texture == nullptr ? "null" : std::fs::path(texture->GetPath()).filename().string() ;

		// Set from selection icon
		if ( ImGui::ButtonIcon( ImGui::IconType::IMAGE, { 16,16 } ) )
		{
			fan::TexturePtr::s_onSetFromSelection.Emmit( _ptr );
		}

		ImGui::SameLine();
		float width = 0.5f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;

		std::fs::path tst = name;
		ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ); ImGui::SameLine();

		// tooltip
		if( texture != nullptr ) {
			fan::gui::ToolTipFast( texture->GetPath().c_str() );
		}

		// dragndrop
//		ImGui::BeginDragDropSourceGameobject( gameobject );
// 		fan::Gameobject * gameobjectDrop = ImGui::BeginDragDropTargetGameobject();
// 		if ( gameobjectDrop ) { ( *_ptr ) = fan::GameobjectPtr( gameobjectDrop, gameobjectDrop->GetUniqueID() ); }

		if ( ImGui::IsItemClicked( 1 ) )
		{
			( *_ptr ) = fan::TexturePtr();
		}

		static std::fs::path m_pathBuffer;
		if ( ImGui::IsItemClicked() )
		{
			ImGui::OpenPopup("set_path_texture");
			m_pathBuffer = "content/models";
		}
		
		if ( fan::gui::LoadFileModal( "set_path_texture",  fan::GlobalValues::s_imagesExtensions, m_pathBuffer ) )
		{
			_ptr->InitUnresolved(m_pathBuffer.string());
		}

		ImGui::SameLine();
		ImGui::Text( _label );

	}
}
