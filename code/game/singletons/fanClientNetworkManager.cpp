#include "game/singletons/fanClientNetworkManager.hpp"

#include "core/fanDebug.hpp"
#include "network/singletons/fanTime.hpp"
#include "game/singletons/fanGame.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/fanSceneSerializable.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/singletons/fanScene.hpp"
#include "network/components/fanClientReplication.hpp"
#include "network/components/fanClientConnection.hpp"
#include "network/singletons/fanLinkingContext.hpp"
#include "network/singletons/fanSpawnManager.hpp"
#include "network/components/fanClientRPC.hpp"
#include "network/components/fanClientGameData.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "network/systems/fanTimeout.hpp"
#include "network/systems/fanClientUpdates.hpp"
#include "network/systems/fanClientSendReceive.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.onGui = &ClientNetworkManager::OnGui;
		_info.name = "client network manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		ClientNetworkManager& netManager = static_cast<ClientNetworkManager&>( _component );
		netManager.persistentHandle = 0;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Start( EcsWorld& _world )
	{
		// Create player persistent scene node
		Scene& scene			= _world.GetSingleton<Scene>();
		SceneNode& persistentNode =  scene.CreateSceneNode( "persistent", &scene.GetRootNode() );
		persistentHandle = persistentNode.handle;
		EcsEntity persistentID	= _world.GetEntity( persistentHandle );
		_world.AddComponent<ReliabilityLayer>( persistentID );
		_world.AddComponent<ClientConnection>( persistentID );
		_world.AddComponent<ClientReplication>( persistentID );
		_world.AddComponent<ClientRPC>( persistentID );
		_world.AddComponent<ClientGameData>( persistentID );

		// connect rpc
		Time& time = _world.GetSingleton<Time>();
		ClientRPC& rpcManager = _world.GetComponent<ClientRPC>( persistentID );
		SpawnManager& spawnManager = _world.GetSingleton<SpawnManager>();
		rpcManager.onShiftFrameIndex.Connect( &ClientGameData::OnShiftFrameIndex, _world, persistentHandle );
		rpcManager.onShiftFrameIndex.Connect( &Time::OnShiftFrameIndex, &time );
		rpcManager.onSpawn.Connect( &SpawnManager::OnSpawn, &spawnManager );

		// Bind socket
		ClientConnection& connection = _world.GetComponent<ClientConnection>( persistentID );
		sf::Socket::Status socketStatus = sf::Socket::Disconnected;
		for( int tryIndex = 0; tryIndex < 10 && socketStatus != sf::Socket::Done; tryIndex++ )
		{
			Debug::Log() << "bind on port " << connection.clientPort << Debug::Endl();
			socketStatus = connection.socket->Bind( connection.clientPort );
			if( socketStatus != sf::Socket::Done )
			{
				Debug::Warning() << " bind failed" << Debug::Endl();
				// try bind on the next port ( useful when using multiple clients on the same machine )
				connection.clientPort++; 
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Stop( EcsWorld& _world )
	{
		const EcsEntity persistentID = _world.GetEntity( persistentHandle );
		ClientConnection& connection = _world.GetComponent<ClientConnection>( persistentID );
		connection.state = ClientConnection::ClientState::Stopping;
		_world.Run<S_ClientSend>( 0.42f );// send a last packet
		connection.socket->Unbind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::OnGui( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		ClientNetworkManager& netManager = static_cast<ClientNetworkManager&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Text( "persistent handle : %d", netManager.persistentHandle );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}