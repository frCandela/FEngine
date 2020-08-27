#include "network/singletons/fanHostManager.hpp"

#include "scene/singletons/fanScene.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "network/components/fanHostGameData.hpp"
#include "network/components/fanHostConnection.hpp"
#include "network/components/fanHostReplication.hpp"
#include "network/components/fanReliabilityLayer.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void HostManager::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::Network16;
		_info.mGroup = EngineGroups::Network;
		_info.onGui  = &HostManager::OnGui;
		_info.mName  = "host manager";
	}

	//========================================================================================================
	//========================================================================================================
	void HostManager::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		HostManager& hostManager = static_cast<HostManager&>( _component );
		hostManager.mHostHandles.clear();
		hostManager.mNetRootNodeHandle = 0;
	}

	//========================================================================================================
	//========================================================================================================
	EcsHandle HostManager::CreateHost( EcsWorld& _world, IpAddress _ip, const Port _port )
	{
		assert( FindHost( _ip, _port ) == 0 );

		// Create an ecs entity associated with the host
		SceneNode& rootNode = _world.GetComponent<SceneNode>( _world.GetEntity( mNetRootNodeHandle ) );
		Scene& scene = _world.GetSingleton<Scene>();
		SceneNode& hostNode = scene.CreateSceneNode( "tmp", &rootNode );
        mHostHandles[{ _ip, _port}] = hostNode.mHandle;
		hostNode.mName = std::string( "host") + std::to_string( hostNode.mHandle );
		const EcsEntity entity = _world.GetEntity( hostNode.mHandle );
		_world.AddComponent< HostGameData >( entity );
		_world.AddComponent< HostReplication >( entity );
		_world.AddComponent< ReliabilityLayer >( entity );

		// fills in the host connection data
		HostConnection& hostConnection = _world.AddComponent< HostConnection >( entity );
		hostConnection.mIp   = _ip;
		hostConnection.mPort = _port;

		return hostNode.mHandle;
	}

	//========================================================================================================
	//========================================================================================================
	void HostManager::DeleteHost( EcsWorld& _world, const EcsHandle _hostHandle )
	{
		// Deletes the ecs entity associated with the host
		const EcsEntity entity = _world.GetEntity( _hostHandle );
		_world.Kill( entity );

		// Delete the host spaceship if spawned
		HostGameData hostGameData = _world.GetComponent<HostGameData>( entity );
		if( hostGameData.mSpaceshipHandle != 0 )
		{
			const EcsEntity spaceshipID = _world.GetEntity( hostGameData.mSpaceshipHandle );
			_world.Kill( spaceshipID );
		}		

		// delete the host ip/port entry
		HostConnection& hostConnection = _world.GetComponent< HostConnection >( entity );
		auto it = mHostHandles.find( { hostConnection.mIp, hostConnection.mPort } );
		assert( it != mHostHandles.end() );
		mHostHandles.erase( it );

		Debug::Log() << "host disconnected "
		             << hostConnection.mIp.toString()
		             << "::"
		             << hostConnection.mPort << Debug::Endl();
	}

	//========================================================================================================
	// returns the client handle associated with an ip/port, returns 0 if it doesn't exists
	//========================================================================================================
	EcsHandle HostManager::FindHost( const sf::IpAddress _ip, const Port _port )
	{
		const auto& it = mHostHandles.find( { _ip, _port});
		return it == mHostHandles.end() ? 0 : it->second;
	}

	//========================================================================================================
	//========================================================================================================
	void HostManager::OnGui( EcsWorld&, EcsSingleton& _component )
	{
        HostManager& hostManager = static_cast<HostManager&>( _component );

        ImGui::Columns( 3 );

        ImGui::Text( "name" );
        ImGui::NextColumn();
        ImGui::Text( "ip" );
        ImGui::NextColumn();
        ImGui::Text( "port" );
        ImGui::NextColumn();
        for( const std::pair<IPPort, EcsHandle>& pair : hostManager.mHostHandles )
        {
            ImGui::Text( "host%d", pair.second );
            ImGui::NextColumn();
            ImGui::Text( "%s", pair.first.mAdress.toString().c_str() );
            ImGui::NextColumn();
            ImGui::Text( "%d", pair.first.mPort );
            ImGui::NextColumn();
        }
        ImGui::Columns( 1 );
	}
}