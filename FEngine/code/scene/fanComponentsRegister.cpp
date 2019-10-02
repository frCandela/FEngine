#include "fanGlobalIncludes.h"
#include "scene/fanComponentsRegister.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	bool ComponentsRegister::RegisterComponent(Component * _component) {
		Get().m_components.push_back(_component);
		return true;
	}
}