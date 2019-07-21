#include "fanIncludes.h"

#include "scene/components/fanComponent.h"
#include "scene/fanGameobject.h"

namespace scene
{	//================================================================================================================================
	//================================================================================================================================
	std::map<uint32_t, const Component * > & Component::Components() {
		static std::map<uint32_t, const Component *  > components;
		return components;
	}

	//================================================================================================================================
	//================================================================================================================================
	Component::Component(Gameobject * _gameobject ) :
		m_gameobject(_gameobject)
		, m_isModified(true)
		, m_isRemovable(true)
		, m_isBeingDeleted(false)
	{
	}

	//================================================================================================================================
	//================================================================================================================================
	void Component::SetModified(const bool _isModified) {
		m_isModified = _isModified;
		if (_isModified == true) {
			GetGameobject()->onComponentModified.Emmit(this);
		}
	}
}