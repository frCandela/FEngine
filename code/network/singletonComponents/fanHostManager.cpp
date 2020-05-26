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
	//================================================================================================================================
	//================================================================================================================================
	void HostManager::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.onGui = &HostManager::OnGui;
		_info.name = "host manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostManager::Init( EcsWorld& _world, EcsSingleton& _component )
	{
		HostManager& hostManager = static_cast<HostManager&>( _component );
		hostManager.hostHandles.clear();
		hostManager.netRoot = nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	EcsHandle HostManager::CreateHost( const sf::IpAddress _ip, const Port _port )
	{
		assert( FindHost( _ip, _port ) == 0 );

		// Create an ecs entity associated with the host
		EcsWorld& world = *netRoot->scene->world;
		SceneNode& hostNode = netRoot->scene->CreateSceneNode( "tmp", netRoot );
		assert( hostHandles.find( { _ip,_port } ) == hostHandles.end() );
		hostHandles[{_ip, _port}] = hostNode.handle;
		hostNode.name = std::string("host") + std::to_string( hostNode.handle );
		const EcsEntity entity = world.GetEntity( hostNode.handle );
		world.AddComponent< HostGameData >( entity );
		world.AddComponent< HostReplication >( entity );
		world.AddComponent< ReliabilityLayer >( entity );

		// fills in the host connection data
		HostConnection& hostConnection = world.AddComponent< HostConnection >( entity );
		hostConnection.ip				= _ip;
		hostConnection.port				= _port;

		return hostNode.handle;
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostManager::DeleteHost( const EcsHandle _hostHandle )
	{
		// Deletes the ecs entity associated with the host
		EcsWorld& world = *netRoot->scene->world;
		const EcsEntity entity = world.GetEntity( _hostHandle );
		world.Kill( entity );

		// Delete the host spaceship if spawned
		HostGameData hostGameData = world.GetComponent<HostGameData>( entity );
		if( hostGameData.spaceshipHandle != 0 )
		{
			const EcsEntity spaceshipID = world.GetEntity( hostGameData.spaceshipHandle );
			world.Kill( spaceshipID );
		}		

		// delete the host ip/port entry
		HostConnection& hostConnection = world.GetComponent< HostConnection >( entity );		
		auto& it = hostHandles.find( { hostConnection.ip, hostConnection.port } );
		assert( it != hostHandles.end() );
		hostHandles.erase( it );

		Debug::Log() << "host disconnected " << hostConnection.ip.toString() << "::" << hostConnection.port << Debug::Endl();
	}

	//================================================================================================================================
	// returns the client handle associated with an ip/port, returns 0 if it doesn't exists
	//================================================================================================================================
	EcsHandle HostManager::FindHost( const sf::IpAddress _ip, const Port _port )
	{
		const auto& it = hostHandles.find({_ip, _port});
		return it == hostHandles.end() ? 0 : it->second;
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostManager::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		HostManager& hostManager = static_cast<HostManager&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Columns( 3 );

			ImGui::Text( "name" );	ImGui::NextColumn();
			ImGui::Text( "ip" );		ImGui::NextColumn();
			ImGui::Text( "port" );		ImGui::NextColumn();
			for( const std::pair<IPPort, EcsHandle>& pair : hostManager.hostHandles )
			{
				ImGui::Text( "host%d", pair.second );						ImGui::NextColumn();
				ImGui::Text( "%s", pair.first.adress.toString().c_str() );	ImGui::NextColumn();
				ImGui::Text( "%d", pair.first.port );						ImGui::NextColumn();
			}
			ImGui::Columns( 1 );

		}ImGui::Unindent(); ImGui::Unindent();
	}
}