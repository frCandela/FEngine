#pragma once

#include "ecs/fanSingletonComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	struct SceneNode;

	//================================================================================================================================
	// [SERVER] Manages host creation/destruction & holds references to the hosts data
	//================================================================================================================================	
	struct HostManager : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );

		//================================================================
		// Hold an ip & port + defines some operators for storing in a unordered_map
		//================================================================		
		struct IPPort
		{
			IpAddress	adress;
			Port		port;
			bool operator==( const IPPort& _ipPort ) const 
			{ 
				return adress == _ipPort.adress && port == _ipPort.port;
			}
			size_t operator()( const IPPort& _ipPort ) const 
			{ 
				return size_t(_ipPort.adress.toInteger()) | ( size_t(_ipPort.port) << 32 );
			}
		};

		std::unordered_map < IPPort, EntityHandle, IPPort > hostHandles;  // links host ip-port to its entity handle
		SceneNode*			netRoot;									  // host entity nodes are placed below the net root node

		EntityHandle CreateHost( const IpAddress _ip, const Port _port );
		void		 DeleteHost( const EntityHandle _hostHandle );
		EntityHandle FindHost( const IpAddress _ip, const Port _port );
	};
}