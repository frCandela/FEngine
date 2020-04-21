#pragma once

#include "fanEcsTypes.hpp"
#include "core/fanHash.hpp"
#include "editor/fanImguiIcons.hpp"

namespace sf { class Packet;  }

namespace fan
{
	//==============================================================================================================================================================
	// A singleton is a component that can only have a single instance per ecs world
	// it can be accessed by systems
	//==============================================================================================================================================================
	struct SingletonComponent
	{
		virtual ~SingletonComponent() {}
	};

	//==============================================================================================================================================================
	// SingletonComponentInfo is runtime type information for singleton components
	// it can be accessed from the ecs world
	// function pointers :
	// init			: clears the component value and registers it when necessary (mandatory)
	// onGui		: draws ui associated with the component (optional)
	// save			: serializes the component to json (optional)
	// load			: deserializes the component from json (optional)
	// netSave		: serializes the component into a packet for replication (optional)
	// netLoad		: deserializes the component from a packet for replication (optional)
	//==============================================================================================================================================================
	struct SingletonComponentInfo
	{
		std::string		name;
		ImGui::IconType icon = ImGui::IconType::NONE;	// editor icon
		uint32_t		staticIndex = 0;				// static index

		void ( *onGui	) ( EcsWorld&, SingletonComponent& ) = nullptr;
		void ( *init	) ( EcsWorld&, SingletonComponent& ) = nullptr;
		void ( *save	) ( const SingletonComponent&, Json& ) = nullptr;
		void ( *load	) ( SingletonComponent&, const Json& ) = nullptr;		
		void ( *netSave ) ( const SingletonComponent&, sf::Packet& _packet ) = nullptr;
		void ( *netLoad ) ( SingletonComponent&,	   sf::Packet& _packet ) = nullptr;
	};

#define DECLARE_SINGLETON_COMPONENT()		\
	private:								\
	friend class EcsWorld;					\
	static const uint32_t s_typeInfo;		\

#define REGISTER_SINGLETON_COMPONENT( _componentType )	\
	const uint32_t _componentType::s_typeInfo = SSID(#_componentType);	
}