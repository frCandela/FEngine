#include "game/singletons/fanClientNetworkManager.hpp"

#include "network/singletons/fanTime.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/singletons/fanScene.hpp"
#include "network/components/fanClientReplication.hpp"
#include "network/components/fanClientConnection.hpp"
#include "network/singletons/fanSpawnManager.hpp"
#include "network/components/fanClientGameData.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "network/systems/fanTimeout.hpp"
#include "network/systems/fanClientUpdates.hpp"
#include "network/systems/fanClientSendReceive.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ClientNetworkManager::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::CLIENT_NET16;
		_info.mGroup = EngineGroups::GameNetwork;
		_info.onGui  = &ClientNetworkManager::OnGui;
		_info.mName  = "client network manager";
	}

	//========================================================================================================
	//========================================================================================================
	void ClientNetworkManager::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		ClientNetworkManager& netManager = static_cast<ClientNetworkManager&>( _component );
		netManager.mPersistentHandle = 0;
	}

	//========================================================================================================
	//========================================================================================================
	void ClientNetworkManager::Start( EcsWorld& _world )
	{
		// Create player persistent scene node
		Scene& scene			= _world.GetSingleton<Scene>();
		SceneNode& persistentNode =  scene.CreateSceneNode( "persistent", &scene.GetRootNode() );
        mPersistentHandle = persistentNode.mHandle;
		EcsEntity persistentID	= _world.GetEntity( mPersistentHandle );
		_world.AddComponent<ReliabilityLayer>( persistentID );
		_world.AddComponent<ClientConnection>( persistentID );
		_world.AddComponent<ClientReplication>( persistentID );
		_world.AddComponent<ClientRPC>( persistentID );
		_world.AddComponent<ClientGameData>( persistentID );

		ClientRPC& rpcManager = _world.GetComponent<ClientRPC>( persistentID );
		ClientConnection& connection = _world.GetComponent<ClientConnection>( persistentID );

		// connect rpc
		Time& time = _world.GetSingleton<Time>();
		SpawnManager& spawnManager = _world.GetSingleton<SpawnManager>();
		rpcManager.mOnShiftFrameIndex.Connect( &ClientGameData::OnShiftFrameIndex, _world, mPersistentHandle );
		rpcManager.mOnShiftFrameIndex.Connect( &Time::OnShiftFrameIndex, &time );
		rpcManager.mOnSpawn.Connect( &SpawnManager::OnSpawn, &spawnManager );
		rpcManager.mOnDespawn.Connect( &SpawnManager::OnDespawn, &spawnManager );
		connection.mOnLoginSuccess.Connect( &ClientGameData::OnLoginSuccess, _world, mPersistentHandle );


		// Bind socket
		sf::Socket::Status socketStatus = sf::Socket::Disconnected;
		for( int tryIndex = 0; tryIndex < 10 && socketStatus != sf::Socket::Done; tryIndex++ )
		{
			Debug::Log() << "bind on port " << connection.mClientPort << Debug::Endl();
			socketStatus = connection.mSocket->Bind( connection.mClientPort );
			if( socketStatus != sf::Socket::Done )
			{
				Debug::Warning() << " bind failed" << Debug::Endl();
				// try bind on the next port ( useful when using multiple clients on the same machine )
				connection.mClientPort++;
			}
		}
	}

	//========================================================================================================
	//========================================================================================================
	void ClientNetworkManager::Stop( EcsWorld& _world )
	{
		const EcsEntity persistentID = _world.GetEntity( mPersistentHandle );
		ClientConnection& connection = _world.GetComponent<ClientConnection>( persistentID );
		connection.mState = ClientConnection::ClientState::Stopping;
		_world.Run<S_ClientSend>( 0.42f );// send a last packet
		connection.mSocket->Unbind();
	}

	//========================================================================================================
	//========================================================================================================
	void ClientNetworkManager::OnGui( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		ClientNetworkManager& netManager = static_cast<ClientNetworkManager&>( _component );
		ImGui::Text( "persistent handle : %d", netManager.mPersistentHandle );
	}
}