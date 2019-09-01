#include "fanGlobalIncludes.h"

#include "scene/components/fanComponent.h"
#include "scene/fanGameobject.h"

namespace fan
{
	namespace scene
	{
		REGISTER_ABSTRACT_TYPE_INFO(Component)

			//================================================================================================================================
			//================================================================================================================================
			Component::Component() :
			m_gameobject(nullptr)
			, m_isModified(true)
			, m_isRemovable(true)
			, m_isBeingDeleted(false) {

		}

		//================================================================================================================================
		//================================================================================================================================
		void Component::SetModified(const bool _isModified) {
			m_isModified = _isModified;
		}
	}
}