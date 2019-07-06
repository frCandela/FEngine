#include "fanIncludes.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanComponent.h"
#include "util/fanSignal.h"

namespace scene
{
	//================================================================================================================================
	//================================================================================================================================
	Gameobject::Gameobject(const std::string _name) :
		m_name( _name )
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	Gameobject::~Gameobject() {
		for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
			delete m_components[componentIndex];
		} m_components.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Gameobject::DeleteComponent(const Component * component)
	{
		// Find the component
		for (int componentIndex = 0; componentIndex < m_components.size(); ++componentIndex)
		{
			if (m_components[componentIndex] == component)
			{
				// Deletes it
				onComponentDeleted.Emmit( m_components[componentIndex] );
				delete(m_components[componentIndex]);
				m_components.erase(m_components.begin() + componentIndex);
				return true;
			}
		}
		return false;
	}


}