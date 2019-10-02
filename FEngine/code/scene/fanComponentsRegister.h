#pragma once

#include "core/fanSingleton.h"

namespace fan
{
	class Component;

	//================================================================================================================================
	// Holds a list of inactive components. Allows editor to access the properties of available components in the game
	//================================================================================================================================
	class ComponentsRegister : public Singleton< ComponentsRegister> {
		friend class Singleton<ComponentsRegister>;
	public:
		static bool	RegisterComponent(Component * _component);
		static std::vector< Component * >&	GetComponents() { return Get().m_components; }

	private:
		std::vector<Component * > m_components;
	};
}

//================================================================================================================================
//================================================================================================================================
#define DECLARE_EDITOR_COMPONENT( _name )	\
	private:								\
	static bool ms_registered;				\

//================================================================================================================================
//================================================================================================================================
#define REGISTER_EDITOR_COMPONENT( _name )									\
	bool _name::ms_registered = ComponentsRegister::RegisterComponent(new _name());	\

