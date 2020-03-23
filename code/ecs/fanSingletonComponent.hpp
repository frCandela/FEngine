#pragma once

#include "fanEcsTypes.hpp"
#include "core/fanHash.hpp"
#include "editor/fanImguiIcons.hpp"

namespace fan
{
	//================================================================================================================================
	// singleton component
	//================================================================================================================================
#define DECLARE_SINGLETON_COMPONENT()		\
	private:								\
	friend class EcsWorld;				\
	static const uint32_t s_typeInfo;		\

#define REGISTER_SINGLETON_COMPONENT( _componentType )	\
	const uint32_t _componentType::s_typeInfo = SSID(#_componentType);	\

	//==============================================================================================================================================================
	// A singleton is a unique component, it can be accessed by systems
	//==============================================================================================================================================================
	struct SingletonComponent {
		virtual ~SingletonComponent(){}
	};

	//==============================================================================================================================================================
	// function pointers :
	// onGui		: draws ui associated with the component
	// init			: clears the component value and registers it when necessary
	// save			: serializes the component to json
	// load			: deserializes the component from json
	//==============================================================================================================================================================
	struct SingletonComponentInfo
	{
		std::string		name;
		ImGui::IconType icon = ImGui::IconType::NONE;	// editor icon
		uint32_t		staticIndex = 0;				// static index

		void		 ( *onGui )( SingletonComponent& ) = nullptr;
		void		 ( *init )( SingletonComponent& ) = nullptr;
		void		 ( *save )( const SingletonComponent&, Json& ) = nullptr;
		void		 ( *load )( SingletonComponent&, const Json& ) = nullptr;		
	};
}