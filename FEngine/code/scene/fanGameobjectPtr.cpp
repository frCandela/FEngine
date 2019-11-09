#include "fanGlobalIncludes.h"
#include "scene/fanGameobjectPtr.h"

#include "scene/fanGameobject.h"
#include "editor/fanImguiIcons.h"
#include "editor/fanModals.h"

namespace fan
{
	Signal< GameobjectPtr * > GameobjectPtr::s_onCreateUnresolved;
	Signal< GameobjectPtr * > GameobjectPtr::s_onSetFromSelection;


	//================================================================================================================================
	//================================================================================================================================
	GameobjectPtr::GameobjectPtr( Gameobject * _gameobject ) :
		m_id( _gameobject->GetUniqueID() ),
		m_gameobject( _gameobject )
	{		
	}

	//================================================================================================================================
	// This constructor requires the scene to manually set its gameobject pointer
	// Useful at loading time
	//================================================================================================================================
	GameobjectPtr::GameobjectPtr( const uint64_t _id ) :
		m_id( _id ),
		m_gameobject( nullptr )
	{
		s_onCreateUnresolved.Emmit( this );	
	}

	//================================================================================================================================
	//================================================================================================================================
	GameobjectPtr::GameobjectPtr() :
		m_id( 0 ),
		m_gameobject( nullptr )
	{
	}
}

namespace ImGui
{
	//================================================================================================================================
	//================================================================================================================================
	void GameobjectPtr( const char * _label, fan::GameobjectPtr * _ptr )
	{
		fan::Gameobject * gameobject = **_ptr;
		
		const ImVec4 color = ImGui::GetStyle().Colors[ImGuiCol_CheckMark];

		std::string name = gameobject != nullptr ? gameobject->GetName() : "null";


		// Set from selection icon
		if ( ImGui::ButtonIcon( ImGui::IconType::GAMEOBJECT16, { 16,16 } ) )
		{
			fan::GameobjectPtr::s_onSetFromSelection.Emmit( _ptr );
		}

		ImGui::SameLine();
		ImGui::Button( name.c_str(), ImVec2(150.f, 0.f) ); ImGui::SameLine();
		// dragndrop
		ImGui::BeginDragDropSourceGameobject( gameobject );
		fan::Gameobject * gameobjectDrop = ImGui::BeginDragDropTargetGameobject();
		if ( gameobjectDrop ) { ( *_ptr ) = fan::GameobjectPtr( gameobjectDrop ); }
		
		ImGui::SameLine();
		ImGui::Text( _label );


		

	}
}
