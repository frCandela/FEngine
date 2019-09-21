#pragma once

#include "core/fanSingleton.h"

namespace fan
{
	namespace scene{ class Component; }

	namespace editor
	{
		//================================================================================================================================
		// Holds a list of inactive components. Allows editor to access the list of available components in the game
		//================================================================================================================================
		class ComponentsRegister : public Singleton< ComponentsRegister> {
			friend class Singleton<ComponentsRegister>;
		public:			
			static bool	RegisterComponent( scene::Component * _component );
			static std::vector< scene::Component * >&	GetComponents() {	return Get().m_components; }

		private:
			std::vector<scene::Component * > m_components;
		};
	}
}

//================================================================================================================================
//================================================================================================================================
#define DECLARE_EDITOR_COMPONENT( _name )	\
	private:								\
	static bool ms_registered;				\

//================================================================================================================================
//================================================================================================================================
#define REGISTER_EDITOR_COMPONENT( _name )									\
	bool _name::ms_registered = editor::ComponentsRegister::RegisterComponent(new _name());	\

