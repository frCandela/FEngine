#include "game/singletonComponents/fanClientNetworkManager.hpp"

#include "core/fanDebug.hpp"
#include "core/time/fanTime.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "game/components/fanPlayerController.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/fanSceneSerializable.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "network/components/fanClientReplication.hpp"
#include "network/components/fanClientConnection.hpp"
#include "network/singletonComponents/fanLinkingContext.hpp"
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
		netManager.shipsToSpawn.clear();
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
		Game& game = _world.GetSingleton<Game>();
		ClientRPC& rpcManager = _world.GetComponent<ClientRPC>( persistentID );
		ClientGameData& gameData = _world.GetComponent<ClientGameData>( persistentID );
		rpcManager.onShiftFrameIndex.Connect( &ClientGameData::OnShiftFrameIndex, &gameData );
		rpcManager.onShiftFrameIndex.Connect( &Game::OnShiftFrameIndex, &game );
		rpcManager.onSpawnClientShip.Connect( &ClientGameData::OnSpawnClientShip, &gameData );
		rpcManager.onSpawnShip.Connect( &ClientNetworkManager::OnSpawnShip, this );

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
		S_ClientSend::Run( _world, _world.Match( S_ClientSend::GetSignature( _world ) ), .42f );// send a last packet
		connection.socket->Unbind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::SpawnShips( EcsWorld& _world )
	{
		const Game& game = _world.GetSingleton<Game>();
		ClientNetworkManager& netManager = _world.GetSingleton<ClientNetworkManager>();
		LinkingContext& linkingContext = _world.GetSingleton<LinkingContext>();

		for (int i = (int)netManager.shipsToSpawn.size() - 1; i >= 0 ; i--)
		{
			std::pair<NetID, FrameIndex> pair = netManager.shipsToSpawn[i];
			if( game.frameIndex >= pair.second )
			{
				// do not spawn twice
				if( linkingContext.netIDToEcsHandle.find( pair.first ) == linkingContext.netIDToEcsHandle.end() )
				{
					// spawn
					const EcsHandle handle = Game::SpawnSpaceship( _world );
					linkingContext.AddEntity( handle, pair.first );
				}
				netManager.shipsToSpawn.erase( netManager.shipsToSpawn.begin() + i );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::OnSpawnShip( NetID _spaceshipID, FrameIndex _frameIndex )
	{
		shipsToSpawn.push_back( {_spaceshipID, _frameIndex} );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::OnGui( EcsWorld& /*_world*/, EcsSingleton& /*_component*/ )
	{
		//ClientNetworkManager& netManager = static_cast<ClientNetworkManager&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}