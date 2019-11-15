#include "fanGlobalIncludes.h"
#include "scene/fanComponentsRegister.h"

#include "scene/components/fanComponent.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	bool ComponentsRegister::RegisterComponent(Component * _component) {
		Get().m_components.push_back(_component);
		return true;
	}

	//================================================================================================================================
	// Get component using a component id
	//================================================================================================================================
	const Component* ComponentsRegister::GetComponent( const uint32_t _componentID )
	{
		for ( int componentIndex = 0; componentIndex < m_components.size(); componentIndex++ )
		{
			Component* component = m_components[componentIndex];

			if ( component->GetType() == _componentID )
			{
				return  component;
			}
		}
		return nullptr;
	}
}