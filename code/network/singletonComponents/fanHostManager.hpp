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

		std::unordered_map< HostID, EntityHandle > hostHandles; // links to the host net entity
		static const int	targetFrameDifference = 7;			// the host must be N frames ahead of the server
		SceneNode*			netRoot;							// host entity nodes are placed below the net root node
		HostID				nextHostID;
		Signal<HostID>		onHostCreated;
		Signal<HostID>		onHostDeleted;

		HostID	CreateHost( const sf::IpAddress _ip, const unsigned short _port );
		void	DeleteHost( const HostID _hostID );
		HostID	FindHost( const sf::IpAddress _ip, const unsigned short _port );
	};
}