#pragma once

#include "core/fanCorePrecompiled.hpp"
#include "core/resources/fanResource.hpp"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	template< typename _ResourceType >
	class ResourceList {
	public:	

	private:
		std::vector< _ResourceType * > m_resources;
	};
}