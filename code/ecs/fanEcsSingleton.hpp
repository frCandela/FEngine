#pragma once

#include <string>
#include "editor/fanImguiIcons.hpp"
#include "core/fanHash.hpp"
#include "fanJson.hpp"
#include "ecs/fanEcsTypes.hpp"

namespace sf
{
	class Packet;
}

namespace fan
{
	//================================
	//================================
	#define ECS_SINGLETON( _SingletonType )										\
	public:																		\
		static constexpr const char* s_name		{ #_SingletonType		  };	\
		static constexpr uint32_t	 s_type		{ SSID( #_SingletonType ) };	

	struct EcsSingleton {
		EcsSingleton(){}
		EcsSingleton( EcsSingleton const& ) = delete;
		EcsSingleton& operator=( EcsSingleton const& ) = delete;
	};
	class EcsWorld;

	//==============================================================================================================================================================
	// EcsSingletonInfo is runtime type information for singleton components
	// Function pointers :
	// init			: clears the component value and registers it when necessary (mandatory)
	// onGui		: draws ui associated with the component (optional)
	// save			: serializes the component to json (optional)
	// load			: deserializes the component from json (optional)
	// netSave		: serializes the component into a packet for replication (optional)
	// netLoad		: deserializes the component from a packet for replication (optional)
	//==============================================================================================================================================================
	struct EcsSingletonInfo
	{
		std::string		name;
		uint32_t		type;
		ImGui::IconType icon = ImGui::IconType::NONE;
		EngineGroups		group = EngineGroups::None;

		void ( *init ) ( EcsWorld&, EcsSingleton& ) = nullptr;
		void ( *onGui ) ( EcsWorld&, EcsSingleton& ) = nullptr;
		void ( *save ) ( const EcsSingleton&, Json& ) = nullptr;
		void ( *load ) ( EcsSingleton&, const Json& ) = nullptr;
		void ( *netSave ) ( const EcsSingleton&, sf::Packet& _packet ) = nullptr;
		void ( *netLoad ) ( EcsSingleton&, sf::Packet& _packet ) = nullptr;
	};
}