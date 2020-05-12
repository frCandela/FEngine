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
	REGISTER_SINGLETON_COMPONENT( ClientNetworkManager );

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.init = &ClientNetworkManager::Init;
		_info.onGui = &ClientNetworkManager::OnGui;
		_info.name = "client network manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		ClientNetworkManager& netManager = static_cast<ClientNetworkManager&>( _component );
		netManager.playerPersistent = nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Start( EcsWorld& _world )
	{
		// Create player persistent scene node
		Scene& scene			= _world.GetSingletonComponent<Scene>();
		playerPersistent		= & scene.CreateSceneNode( "persistent", scene.root );
		EntityID persistentID	= _world.GetEntityID( playerPersistent->handle );
		_world.AddComponent<ReliabilityLayer>( persistentID );
		_world.AddComponent<ClientConnection>( persistentID );
		_world.AddComponent<ClientReplication>( persistentID );
		_world.AddComponent<ClientRPC>( persistentID );
		_world.AddComponent<ClientGameData>( persistentID );

		// connect rpc
		Game& game = _world.GetSingletonComponent<Game>();
		ClientRPC& rpcManager = _world.GetComponent<ClientRPC>( persistentID );
		ClientGameData& gameData = _world.GetComponent<ClientGameData>( persistentID );
		rpcManager.onShiftFrameIndex.Connect( &ClientGameData::OnShiftFrameIndex, &gameData );
		rpcManager.onShiftFrameIndex.Connect( &Game::OnShiftFrameIndex, &game );
		rpcManager.onSpawnClientShip.Connect( &ClientGameData::OnSpawnShip, &gameData );
		rpcManager.onSpawnShip.Connect( &ClientNetworkManager::OnSpawnShip, this );

		// Bind socket
		ClientConnection& connection = _world.GetComponent<ClientConnection>( persistentID );
		sf::Socket::Status socketStatus = sf::Socket::Disconnected;
		for( int tryIndex = 0; tryIndex < 10 && socketStatus != sf::Socket::Done; tryIndex++ )
		{
			Debug::Log() << "bind on port " << connection.clientPort << Debug::Endl();
			socketStatus = connection.socket.Bind( connection.clientPort );
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
		const EntityID persistentID = _world.GetEntityID( playerPersistent->handle );		
		ClientConnection& connection = _world.GetComponent<ClientConnection>( persistentID );
		connection.state = ClientConnection::ClientState::Stopping;
		S_ClientSend::Run( _world, _world.Match( S_ClientSend::GetSignature( _world ) ), .42f );// send a last packet
		connection.socket.Unbind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::OnSpawnShip( NetID _spaceshipID, FrameIndex _frameIndex )
	{
		Debug::Highlight() << "spawn " << _spaceshipID << " " << _frameIndex << Debug::Endl();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		ClientNetworkManager& netManager = static_cast<ClientNetworkManager&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}