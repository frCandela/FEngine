#include "fanIncludes.h"

#include "scene/components/fanComponent.h"

namespace scene
{
	Component::Component(Gameobject * _gameobject ) :
		m_gameobject(_gameobject)
		, m_isModified(true)
		, m_isRemovable(true)
	{

	}
}