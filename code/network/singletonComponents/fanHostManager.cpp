#include "network/singletonComponents/fanHostManager.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "network/components/fanHostGameData.hpp"
#include "network/components/fanHostConnection.hpp"
#include "network/components/fanHostReplication.hpp"
#include "network/components/fanHostDeliveryNotification.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( HostManager );

	//================================================================================================================================
	//================================================================================================================================
	void HostManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.init = &HostManager::Init;
		_info.onGui = &HostManager::OnGui;
		_info.name = "host manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostManager::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		HostManager& hostManager = static_cast<HostManager&>( _component );
		hostManager.hostHandles.clear();
		hostManager.netRoot = nullptr;
		hostManager.nextHostID = 1;
		hostManager.onHostCreated.Clear();
		hostManager.onHostDeleted.Clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	HostID HostManager::CreateHost( const sf::IpAddress _ip, const unsigned short _port )
	{
		assert( FindHost( _ip, _port ) == -1 );

		// Create an ecs entity associated with the host
		EcsWorld& world = *netRoot->scene->world;
		const HostID hostID = nextHostID++;
		const SceneNode& hostNode = netRoot->scene->CreateSceneNode( "host" + std::to_string( hostID ), netRoot );
		assert( hostHandles.find( hostID ) == hostHandles.end() );
		hostHandles[hostID] = hostNode.handle;
		const EntityID entityID = world.GetEntityID( hostNode.handle );
		world.AddComponent< HostGameData >( entityID );
		world.AddComponent< HostReplication >( entityID );
		world.AddComponent< HostDeliveryNotification >( entityID );
		HostConnection& hostConnection = world.AddComponent< HostConnection >( entityID );

		// fills in the host connection data
		hostConnection.ip = _ip;
		hostConnection.port = _port;
		hostConnection.name = "Unknown";
		hostConnection.state = HostConnection::Disconnected;
		hostConnection.rtt = 0.f;
		hostConnection.lastResponseTime = 0.f;
		hostConnection.lastPingTime = 0.f;

		onHostCreated.Emmit( hostID );

		return hostID;
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostManager::DeleteHost( const HostID _hostID )
	{
		// Deletes the ecs entity associated with the host
		EcsWorld& world = *netRoot->scene->world;
		const EntityHandle hostHandle = hostHandles[_hostID];
		const EntityID hostID = world.GetEntityID( hostHandle );
		world.KillEntity( hostID );
		hostHandles.erase( _hostID );

		Debug::Log() << "host " << _hostID << " disconnected " << Debug::Endl();
		onHostDeleted.Emmit( _hostID );
	}

	//================================================================================================================================
	// returns the client data associated with an ip/port, returns nullptr if it doesn't exists
	//================================================================================================================================
	HostID HostManager::FindHost( const sf::IpAddress _ip, const unsigned short _port )
	{
		for( std::pair<HostID, EntityHandle> pair : hostHandles )
		{
			EcsWorld& world = *netRoot->scene->world;
			const HostID   hostID = pair.first;
			const EntityID entityID = world.GetEntityID( pair.second );
			HostConnection& hostConnection = world.GetComponent<HostConnection>( entityID );

			if( hostConnection.ip == _ip && hostConnection.port == _port )
			{
				return hostID;
			}
		}
		return -1;
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostManager::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		HostManager& hostManager = static_cast<HostManager&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{

		}ImGui::Unindent(); ImGui::Unindent();
	}
}