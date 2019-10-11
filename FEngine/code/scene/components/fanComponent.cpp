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
		  m_lastModified(0)
		, m_isRemovable(true)
		, m_isBeingDeleted(false) {

	}

	//================================================================================================================================
	//================================================================================================================================
	void Component::OnAttach() {};
	void Component::OnDetach() {}
	void Component::OnGui() {}

	//================================================================================================================================
	//================================================================================================================================
	bool Component::IsModified() const {
		return Input::GetFrameCount() == m_lastModified;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Component::MarkModified(const bool _updateAABB) {
		m_lastModified = Input::GetFrameCount();
		if (_updateAABB) {
			m_gameobject->GetScene()->ComputeAABBEndFrame(m_gameobject);
		}
	}
}