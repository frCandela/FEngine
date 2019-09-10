#include "fanGlobalIncludes.h"

#include "scene/components/fanComponent.h"
#include "scene/fanEntity.h"

namespace fan
{
	namespace scene
	{
		fan::Signal< Component * > Component::onComponentCreated;
		fan::Signal< Component * > Component::onComponentModified;
		fan::Signal< Component * > Component::onComponentDeleted;
		REGISTER_ABSTRACT_TYPE_INFO(Component)

		//================================================================================================================================
		//================================================================================================================================
		Component::Component() :
			m_entity(nullptr)
			, m_isModified(true)
			, m_isRemovable(true)
			, m_isBeingDeleted(false) {

		}

		//================================================================================================================================
		//================================================================================================================================
		void Component::OnAttach() {
			onComponentCreated.Emmit(this);
		};

		//================================================================================================================================
		//================================================================================================================================
		void Component::OnDetach() {
			onComponentDeleted.Emmit(this);
		}

		//================================================================================================================================
		//================================================================================================================================
		void Component::SetModified(const bool _isModified) {
			m_isModified = _isModified;
		}
	}
}