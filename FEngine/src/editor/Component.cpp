#include "editor/Component.h"

bool Component::WasModified()
{
	if (m_wasModified)
	{
		m_wasModified = false;
		return true;
	}
	return false;
}
