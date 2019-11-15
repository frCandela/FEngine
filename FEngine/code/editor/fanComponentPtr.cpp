#include "fanGlobalIncludes.h"
#include "editor/fanComponentPtr.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanComponent.h"
#include "editor/fanDragnDrop.h"

namespace ImGui
{
	bool FanComponent( const char * _label, const uint32_t _typeID, fan::ComponentIDPtr * _ptr )
	{
		bool returnValue = false;

 		fan::Component * component = **_ptr;
 		const std::string name = component != nullptr ? ( ( std::string( component->GetName() ) + ": " ) + component->GetGameobject()->GetName() ) : "null";
 		const fan::Component * componentSample = fan::TypeInfo::Get().GetInstance<fan::Component>(_typeID );

		// icon & set from selection
		if ( ImGui::ButtonIcon( componentSample->GetIcon(), { 16,16 } ) )
		{
			returnValue = true;
		}

		ImGui::SameLine();

		// name button 
		float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
		ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ); ImGui::SameLine();
		ImGui::SameLine();

		// dragndrop
		ImGui::FanBeginDragDropSourceComponent( component );
		fan::Component * componentDrop = ImGui::FanBeginDragDropTargetComponent( _typeID );
		if ( componentDrop )
		{
			( *_ptr ) = fan::ComponentIDPtr( componentDrop, fan::IDPtrData( componentDrop->GetGameobject()->GetUniqueID(), componentDrop->GetType() ) );
			returnValue = true;
		}

		// Right click = clear
		if ( ImGui::IsItemClicked( 1 ) )
		{
			( *_ptr ) = fan::ComponentIDPtr();
			returnValue = true;
		}

		// label	
		ImGui::Text( _label );

		return returnValue;
	}
}