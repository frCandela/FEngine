#include "fanGlobalIncludes.h"

#include "scene/components/fanComponent.h"
#include "scene/fanGameobject.h"
#include "scene/fanScene.h"
#include "core/input/fanInput.h"

namespace fan
{
	REGISTER_ABSTRACT_TYPE_INFO(Component)

	//================================================================================================================================
	//================================================================================================================================
	Component::Component() :
		m_isRemovable(true)
		, m_isBeingDeleted(false) {

	}

	//================================================================================================================================
	//================================================================================================================================
	void Component::OnAttach() {};
	void Component::OnDetach() {}
	void Component::OnGui() {}

	//================================================================================================================================
	//================================================================================================================================
	bool Component::Save( Json & _json ) const {
		SaveUInt( _json, "id", GetType() );
		SaveString( _json, "type", GetName() );
		return true;
	}
}