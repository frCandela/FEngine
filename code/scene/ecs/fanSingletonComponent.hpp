#pragma once

#include "fanEcsTypes.hpp"
#include "core/fanHash.hpp"

namespace fan
{
	//================================================================================================================================
	// singleton component
	//================================================================================================================================
#define DECLARE_SINGLETON_COMPONENT()		\
	private:								\
	friend class EntityWorld;				\
	static const uint32_t s_typeInfo;		\
	static const char* s_typeName;			\

#define REGISTER_SINGLETON_COMPONENT( _componentType, _name)	\
	const uint32_t _componentType::s_typeInfo = SSID(#_name);	\
	const char* _componentType::s_typeName = _name;				\

	//==============================================================================================================================================================
	// A singleton is a unique component, it can be accessed by systems
	//==============================================================================================================================================================
	struct SingletonComponent {};

	//==============================================================================================================================================================
	// tmp test 
	//==============================================================================================================================================================
	struct sc_sunLight : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		int test = 0;
	};
}