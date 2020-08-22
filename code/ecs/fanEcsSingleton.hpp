#pragma once

#include <string>
#include "editor/fanImguiIcons.hpp"
#include "core/fanHash.hpp"
#include "fanJson.hpp"
#include "ecs/fanEcsTypes.hpp"

namespace sf { class Packet; }

namespace fan
{
    //========================================================================================================
    //========================================================================================================
	#define ECS_SINGLETON( _SingletonType )										\
	public:																		\
		static constexpr const char* s_name		{ #_SingletonType		  };	\
		static constexpr uint32_t	 s_type		{ SSID( #_SingletonType ) };	

    //========================================================================================================
    //========================================================================================================
    struct EcsSingleton {
		EcsSingleton(){}
		EcsSingleton( EcsSingleton const& ) = delete;
		EcsSingleton& operator=( EcsSingleton const& ) = delete;
	};

	class EcsWorld;
    struct SlotBase;

	//========================================================================================================
	// EcsSingletonInfo is runtime type information for singleton components
	// Function pointers :
	// init			: clears the component value and registers it when necessary (mandatory)
    // start		: called once at the start of the game
	// onGui		: draws ui associated with the component
	// save			: serializes the component to json
	// load			: deserializes the component from json
	// netSave		: serializes the component into a packet for replication
	// netLoad		: deserializes the component from a packet for replication
	//========================================================================================================
	struct EcsSingletonInfo
	{
		std::string		        name;
		uint32_t		        type;
		ImGui::IconType         icon = ImGui::IconType::NONE;
		EngineGroups	        group = EngineGroups::None;
        std::vector<SlotBase*>  mSlots;                         // callable methods

		void ( *init ) ( EcsWorld&, EcsSingleton& ) = nullptr;
        void ( *setInfo ) ( EcsSingletonInfo& ) = nullptr;
		void ( *onGui ) ( EcsWorld&, EcsSingleton& ) = nullptr;
		void ( *save ) ( const EcsSingleton&, Json& ) = nullptr;
		void ( *load ) ( EcsSingleton&, const Json& ) = nullptr;
		void ( *netSave ) ( const EcsSingleton&, sf::Packet& _packet ) = nullptr;
		void ( *netLoad ) ( EcsSingleton&, sf::Packet& _packet ) = nullptr;
	};
}