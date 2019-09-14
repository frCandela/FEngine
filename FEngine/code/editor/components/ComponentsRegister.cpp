#include "fanGlobalIncludes.h"
#include "editor/components/ComponentsRegister.h"

namespace fan
{
	namespace editor
	{
		//================================================================================================================================
		//================================================================================================================================
		bool ComponentsRegister::RegisterComponent(scene::Component * _component) {
			Get().m_components.push_back(_component);
			return true;
		}
	}

}