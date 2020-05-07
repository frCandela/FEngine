#include "network/singletonComponents/fanHostManager.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "network/components/fanHostGameData.hpp"
#include "network/components/fanHostConnection.hpp"
#include "network/components/fanHostReplication.hpp"
#include "network/components/fanReliabilityLayer.hpp"

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
	}

	//================================================================================================================================
	//================================================================================================================================
	EntityHandle HostManager::CreateHost( const sf::IpAddress _ip, const Port _port )
	{
		assert( FindHost( _ip, _port ) == 0 );

		// Create an ecs entity associated with the host
		EcsWorld& world = *netRoot->scene->world;
		SceneNode& hostNode = netRoot->scene->CreateSceneNode( "tmp", netRoot );
		assert( hostHandles.find( { _ip,_port } ) == hostHandles.end() );
		hostHandles[{_ip, _port}] = hostNode.handle;
		hostNode.name = std::string("host") + std::to_string( hostNode.handle );
		const EntityID entityID = world.GetEntityID( hostNode.handle );
		world.AddComponent< HostGameData >( entityID );
		world.AddComponent< HostReplication >( entityID );
		world.AddComponent< ReliabilityLayer >( entityID );

		// fills in the host connection data
		HostConnection& hostConnection = world.AddComponent< HostConnection >( entityID );
		hostConnection.ip				= _ip;
		hostConnection.port				= _port;

		return hostNode.handle;
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostManager::DeleteHost( const EntityHandle _hostHandle )
	{
		// Deletes the ecs entity associated with the host
		EcsWorld& world = *netRoot->scene->world;
		const EntityID entityID = world.GetEntityID( _hostHandle );
		world.KillEntity( entityID );

		// Delete the host spaceship if spawned
		HostGameData hostGameData = world.GetComponent<HostGameData>( entityID );
		if( hostGameData.spaceshipHandle != 0 )
		{
			const EntityID spaceshipID = world.GetEntityID( hostGameData.spaceshipHandle );
			world.KillEntity( spaceshipID );
		}		

		// delete the host ip/port entry
		HostConnection& hostConnection = world.GetComponent< HostConnection >( entityID );		
		auto& it = hostHandles.find( { hostConnection.ip, hostConnection.port } );
		assert( it != hostHandles.end() );
		hostHandles.erase( it );

		Debug::Log() << "host disconnected " << hostConnection.ip.toString() << "::" << hostConnection.port << Debug::Endl();
	}

	//================================================================================================================================
	// returns the client handle associated with an ip/port, returns 0 if it doesn't exists
	//================================================================================================================================
	EntityHandle HostManager::FindHost( const sf::IpAddress _ip, const Port _port )
	{
		const auto& it = hostHandles.find({_ip, _port});
		return it == hostHandles.end() ? 0 : it->second;
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostManager::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		HostManager& hostManager = static_cast<HostManager&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Columns( 3 );

			ImGui::Text( "name" );	ImGui::NextColumn();
			ImGui::Text( "ip" );		ImGui::NextColumn();
			ImGui::Text( "port" );		ImGui::NextColumn();
			for( const std::pair<IPPort, EntityHandle>& pair : hostManager.hostHandles )
			{
				ImGui::Text( "host%d", pair.second );						ImGui::NextColumn();
				ImGui::Text( "%s", pair.first.adress.toString().c_str() );	ImGui::NextColumn();
				ImGui::Text( "%d", pair.first.port );						ImGui::NextColumn();
			}
			ImGui::Columns( 1 );

		}ImGui::Unindent(); ImGui::Unindent();
	}
}