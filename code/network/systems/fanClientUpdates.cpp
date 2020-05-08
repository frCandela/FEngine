#include "network/systems/fanClientUpdates.hpp"

#include "ecs/fanEcsWorld.hpp"
// #include "core/time/fanTime.hpp"
// #include "scene/components/fanSceneNode.hpp"
#include "network/singletonComponents/fanLinkingContext.hpp"
#include "network/components/fanClientReplication.hpp"
#include "network/components/fanClientConnection.hpp"
#include "network/components/fanClientGameData.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/components/fanPlayerController.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_ClientNetworkUpdate::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<ClientConnection>() |
			_world.GetSignature<ClientGameData>()   |
			_world.GetSignature<ClientReplication>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ClientNetworkUpdate::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		LinkingContext& linkingContext = _world.GetSingletonComponent<LinkingContext>();

		for( EntityID entityID : _entities )
		{
			ClientConnection& connection = _world.GetComponent<ClientConnection>( entityID );
			ClientReplication& replication = _world.GetComponent<ClientReplication>( entityID );
			ClientGameData& gameData = _world.GetComponent<ClientGameData>( entityID );

			Game& game = _world.GetSingletonComponent<Game>();

			replication.ReplicateSingletons( _world );

			// spawns spaceship
			if( gameData.spaceshipSpawnFrameIndex != 0 && game.frameIndex >= gameData.spaceshipSpawnFrameIndex )
			{
				assert( gameData.spaceshipNetID != 0 );

				gameData.spaceshipSpawnFrameIndex = 0;
				gameData.spaceshipHandle = Game::SpawnSpaceship( _world );
				linkingContext.AddEntity( gameData.spaceshipHandle, gameData.spaceshipNetID );

				const EntityID spaceshipID = _world.GetEntityID( gameData.spaceshipHandle );
				if( gameData.spaceshipHandle != 0 )
				{
					_world.AddComponent<PlayerController>( spaceshipID );
				}
			}

			if( gameData.spaceshipHandle != 0 && gameData.synced )
			{
				const EntityID entityID = _world.GetEntityID( gameData.spaceshipHandle );
				const PlayerInput& input = _world.GetComponent<PlayerInput>( entityID );

				// streams input to the server
				PacketInput packetInput;
				packetInput.frameIndex = game.frameIndex;
				packetInput.orientation = input.orientation;
				packetInput.left = input.left;
				packetInput.forward = input.forward;
				packetInput.boost = input.boost;
				packetInput.fire = input.fire;
				gameData.inputs.push_front( packetInput );

				// saves previous player state
				const Rigidbody& rb = _world.GetComponent<Rigidbody>( entityID );
				const Transform& transform = _world.GetComponent<Transform>( entityID );
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
	Signature S_ClientNetworkSend::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<ReliabilityLayer>() |
			_world.GetSignature<ClientConnection>() |
			_world.GetSignature<ClientGameData>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ClientNetworkSend::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		Game& game = _world.GetSingletonComponent<Game>();

		for( EntityID entityID : _entities )
		{
			ReliabilityLayer& reliabilityLayer = _world.GetComponent<ReliabilityLayer>( entityID );
			ClientConnection& connection = _world.GetComponent<ClientConnection>( entityID );
			ClientGameData& gameData = _world.GetComponent<ClientGameData>( entityID );

			// create packet
			Packet packet( reliabilityLayer.GetNextPacketTag() );

			// write packet
			connection.Write( packet );
			gameData.Write( packet );

			if( packet.GetSize() == sizeof( PacketTag ) ) { packet.onlyContainsAck = true; }

			reliabilityLayer.Write( packet );

			// send packet, don't send empty packets
			if( packet.GetSize() > sizeof( PacketTag ) )
			{
				reliabilityLayer.RegisterPacket( packet );
				connection.bandwidth = 1.f / game.logicDelta * float( packet.GetSize() ) / 1000.f; // in Ko/s
				connection.socket.Send( packet, connection.serverIP, connection.serverPort );
			}
			else
			{
				reliabilityLayer.nextPacketTag--;
			}
		}
	}
}