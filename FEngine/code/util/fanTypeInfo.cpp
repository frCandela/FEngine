#include "fanIncludes.h"

#include "util/fanTypeInfo.h"

std::map<uint32_t, std::function<void*()>> & TypeInfo::m_constructors(){
	static std::map<uint32_t, std::function<void*()>> constructors;
	return constructors;
}