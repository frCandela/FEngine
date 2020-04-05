#pragma once

#include "fanEcsTypes.hpp"
#include "core/fanHash.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// A Tag is a component with no data ( basically a bit in a signature )
	// it is used to change the signature of entities for more fine grained system calls
	//==============================================================================================================================================================
	struct Tag {};

	//================================================================================================================================
	// tag macros
	//================================================================================================================================
#define DECLARE_TAG()													\
	public:																\
	static const uint32_t s_typeInfo;									\
	static const char* s_typeName;										\

#define REGISTER_TAG( _componentType, _name)					\
	const uint32_t _componentType::s_typeInfo = SSID(#_name);	\
	const char* _componentType::s_typeName = _name;				
}