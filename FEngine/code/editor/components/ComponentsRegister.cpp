#include "fanGlobalIncludes.h"
#include "editor/components/ComponentsRegister.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	bool ComponentsRegister::RegisterComponent(Component * _component) {
		Get().m_components.push_back(_component);
		return true;
	}
}