#include "network/systems/fanClientUpdates.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanTime.hpp"
#include "network/singletonComponents/fanLinkingContext.hpp"
#include "network/components/fanClientReplication.hpp"
#include "network/components/fanClientConnection.hpp"
#include "network/components/fanClientGameData.hpp"
#include "network/components/fanClientRPC.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/components/fanPlayerController.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_ClientSpawnSpaceship::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<ClientConnection>() |
			_world.GetSignature<ClientGameData>()   |
			_world.GetSignature<ClientReplication>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ClientSpawnSpaceship::Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		LinkingContext& linkingContext = _world.GetSingleton<LinkingContext>();
		const Game& game = _world.GetSingleton<Game>();

		for( EcsEntity entity : _entities )
		{			
			ClientGameData& gameData = _world.GetComponent<ClientGameData>( entity );

			// spawns spaceship
			if( gameData.spaceshipSpawnFrameIndex != 0 
				&& game.frameIndex >= gameData.spaceshipSpawnFrameIndex
				&& gameData.spaceshipHandle == 0 )
			{
				assert( gameData.spaceshipNetID != 0 );

				gameData.spaceshipSpawnFrameIndex = 0;
				gameData.spaceshipHandle = Game::SpawnSpaceship( _world );
				linkingContext.AddEntity( gameData.spaceshipHandle, gameData.spaceshipNetID );

				const EcsEntity spaceshipID = _world.GetEntity( gameData.spaceshipHandle );
				if( gameData.spaceshipHandle != 0 )
				{
					_world.AddComponent<PlayerController>( spaceshipID );
				}
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_ClientSaveState::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<ClientGameData>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ClientSaveState::Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		const Game& game = _world.GetSingleton<Game>();

		for( EcsEntity entity : _entities )
		{
			ClientGameData& gameData = _world.GetComponent<ClientGameData>( entity );

			if( gameData.spaceshipHandle != 0 && gameData.frameSynced )
			{
				const EcsEntity spaceshipID = _world.GetEntity( gameData.spaceshipHandle );

				// saves previous player state
				const Rigidbody& rb = _world.GetComponent<Rigidbody>( spaceshipID );
				const Transform& transform = _world.GetComponent<Transform>( spaceshipID );
				assert( rb.rigidbody.getTotalForce().isZero() );
				PacketPlayerGameState playerState;
				playerState.frameIndex = game.frameIndex;
				playerState.position = transform.GetPosition();
				playerState.orientation = transform.GetRotationEuler();
				playerState.velocity = rb.GetVelocity();
				playerState.angularVelocity = rb.GetAngularVelocity();
				gameData.previousStates.push( playerState );
			}
		}
	}


	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_ClientRunReplication::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<ClientReplication>() | _world.GetSignature<ClientRPC>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ClientRunReplication::Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		LinkingContext& linkingContext = _world.GetSingleton<LinkingContext>();

		for( EcsEntity entity : _entities )
		{
			ClientReplication&	replication = _world.GetComponent<ClientReplication>( entity );
			ClientRPC&			rpc			= _world.GetComponent<ClientRPC>( entity );

			// replicate singletons components
			for( PacketReplication packet : replication.replicationListSingletons )
			{
				sf::Uint32 staticIndex;
				packet.packetData >> staticIndex;
				SingletonComponent& singleton = _world.GetSingleton( staticIndex );
				const EcsSingletonInfo& info = _world.GetSingletonEcsComponentInfo( staticIndex );
				info.netLoad( singleton, packet.packetData );
				assert( packet.packetData.endOfPacket() );
			}
			replication.replicationListSingletons.clear();

			// replicate entities
			for( PacketReplication packet : replication.replicationListEntities )
			{
				NetID netID;
				sf::Uint8 numComponents;

				packet.packetData >> netID;
				packet.packetData >> numComponents;

				auto it = linkingContext.netIDToEcsHandle.find( netID );
				if( it != linkingContext.netIDToEcsHandle.end() )
				{
					const EcsHandle replicatedHandle = it->second;
					const EcsEntity replicatedID = _world.GetEntity( replicatedHandle );

					for( int i = 0; i < numComponents; i++ )
					{
						sf::Uint32 staticIndex;
						packet.packetData >> staticIndex;
						const int dynamicIndex = _world.GetIndex( staticIndex );
						const EcsComponentInfo& info = _world.GetComponentInfo( dynamicIndex );
						EcsComponent& component = _world.GetComponent( replicatedID, dynamicIndex );
						info.netLoad( component, packet.packetData );
					}
				}
			}
			replication.replicationListEntities.clear();

			// replicate RPC
			for( PacketReplication packet : replication.replicationListRPC )
			{
				rpc.TriggerRPC( packet.packetData );
			}
			replication.replicationListRPC.clear();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_ClientDetectServerTimeout::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<ClientConnection>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ClientDetectServerTimeout::Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		for( EcsEntity entity : _entities )
		{
			ClientConnection& connection = _world.GetComponent<ClientConnection>( entity );
			
			if( connection.state == ClientConnection::ClientState::Connected )
			{
				const double currentTime = Time::Get().ElapsedSinceStartup();
				if( connection.serverLastResponse + connection.timeoutDelay < currentTime )
				{
					Debug::Log() << "server timeout " << Debug::Endl();
					connection.state = ClientConnection::ClientState::Disconnected;
				}
			}
		}
	}





	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_ClientSaveInput::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<ClientGameData>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ClientSaveInput::Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		const Game& game = _world.GetSingleton<Game>();

		for( EcsEntity entity : _entities )
		{
			ClientGameData& gameData = _world.GetComponent<ClientGameData>( entity );

			if( gameData.spaceshipHandle != 0 && gameData.frameSynced )
			{
				const EcsEntity spaceshipID = _world.GetEntity( gameData.spaceshipHandle );
				const PlayerInput& input = _world.GetComponent<PlayerInput>( spaceshipID );

				// streams input to the server
				PacketInput::InputData inputData;
				inputData.frameIndex = game.frameIndex;
				inputData.orientation = sf::Vector2f( input.orientation.x(), input.orientation.z() );
				inputData.left = input.left > 0;
				inputData.right = input.left < 0;
				inputData.forward = input.forward > 0;
				inputData.backward = input.forward < 0;
				inputData.boost = input.boost > 0;
				inputData.fire = input.fire > 0;
				gameData.previousInputs.push_front( inputData );
				gameData.previousInputsSinceLastGameState.push_front( inputData );
			}
		}
	}
}