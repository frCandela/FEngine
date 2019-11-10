#include "fanGlobalIncludes.h"
#include "scene/fanRessourcePtr.h"

#include "scene/fanGameobject.h"
#include "editor/fanImguiIcons.h"
#include "editor/fanModals.h"

namespace ImGui
{
	//================================================================================================================================
	//================================================================================================================================
	void InputGameobject( const char * _label,  fan::GameobjectPtr * _ptr )
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
		float width = 0.5f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;



		ImGui::Button( name.c_str(), ImVec2( width, 0.f) ); ImGui::SameLine();
		// dragndrop
		ImGui::BeginDragDropSourceGameobject( gameobject );
		fan::Gameobject * gameobjectDrop = ImGui::BeginDragDropTargetGameobject();
		if ( gameobjectDrop ) { ( *_ptr ) =  fan::GameobjectPtr( gameobjectDrop, gameobjectDrop->GetUniqueID() ); }
		
		if ( ImGui::IsItemClicked( 1 ) )
		{
			fan::Debug::Log("bwa");
			(*_ptr ) =  fan::GameobjectPtr();
		}


		ImGui::SameLine();
		ImGui::Text( _label );

	}
}
