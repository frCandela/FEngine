#include "fanGlobalIncludes.h"

#include "core/fanTypeInfo.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	std::map<uint32_t, std::function<void*()>> & TypeInfo::m_constructors() {
		static std::map<uint32_t, std::function<void*()>> constructors;
		return constructors;
	}
}