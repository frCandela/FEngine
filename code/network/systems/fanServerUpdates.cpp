#include "network/systems/fanServerUpdates.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanTime.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "network/singletonComponents/fanLinkingContext.hpp"
#include "network/components/fanClientRPC.hpp"
#include "network/singletonComponents/fanHostManager.hpp"
#include "network/singletonComponents/fanServerConnection.hpp"
#include "network/components/fanHostConnection.hpp"
#include "network/components/fanHostReplication.hpp"
#include "network/components/fanHostGameData.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_ServerUpdateHosts::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<HostConnection>() |
			_world.GetSignature<HostGameData>()   |
			_world.GetSignature<HostReplication>();
	}

	//================================================================================================================================
	// @todo split this in multiple systems
	//================================================================================================================================
	void S_ServerUpdateHosts::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		LinkingContext&		linkingContext	= _world.GetSingletonComponent<LinkingContext>();
		const HostManager&	hostManager		= _world.GetSingletonComponent<HostManager>();
		const Game&			game			= _world.GetSingletonComponent<Game>();

		for( EntityID entityID : _entities )
		{
			HostConnection&  hostConnection  = _world.GetComponent< HostConnection >( entityID );
			HostGameData&	 hostData		 = _world.GetComponent< HostGameData >( entityID );
			HostReplication& hostReplication = _world.GetComponent< HostReplication >( entityID );

			if( hostConnection.state == HostConnection::Connected )
			{
				if( hostConnection.synced == true )
				{
					if( hostData.spaceshipID == 0 )
					{
						// spawns spaceship
						hostData.spaceshipHandle = Game::SpawnSpaceship( _world );
						hostData.spaceshipID = linkingContext.nextNetID++;
						linkingContext.AddEntity( hostData.spaceshipHandle, hostData.spaceshipID );

						hostReplication.Replicate(
							ClientRPC::RPCSSpawnShip( hostData.spaceshipID, game.frameIndex + 120 )
							, HostReplication::ResendUntilReplicated
						);
					}

					if( hostData.spaceshipID != 0 )
					{
						// get the current input for this client
						while( !hostData.inputs.empty() )
						{
							const PacketInput::InputData& inputData = hostData.inputs.front();
							if( inputData.frameIndex < game.frameIndex || inputData.frameIndex > game.frameIndex + 60 )
							{
								hostData.inputs.pop();
							}
							else
							{
								break;
							}
						}

						// moves spaceship						
						if( !hostData.inputs.empty() && hostData.inputs.front().frameIndex == game.frameIndex )
						{
							const PacketInput::InputData& inputData = hostData.inputs.front();
							hostData.inputs.pop();

							const EntityID shipEntityID = _world.GetEntityID( hostData.spaceshipHandle );
							PlayerInput& input = _world.GetComponent<PlayerInput>( shipEntityID );
							input.orientation = btVector3( inputData.orientation.x, 0.f, inputData.orientation.y );
							input.left		= inputData.left	? 1.f : ( inputData.right	 ? -1.f : 0.f );
							input.forward	= inputData.forward ? 1.f : ( inputData.backward ? -1.f : 0.f );
							input.boost		= inputData.boost;
							input.fire		= inputData.fire;
						}
						else
						{
							Debug::Warning() << "no available input from player " << Debug::Endl();
						}

						// generate player state
						{
							const EntityID shipEntityID = _world.GetEntityID( hostData.spaceshipHandle );
							const Rigidbody& rb = _world.GetComponent<Rigidbody>( shipEntityID );
							const Transform& transform = _world.GetComponent<Transform>( shipEntityID );
							hostData.nextPlayerState.frameIndex = game.frameIndex;
							hostData.nextPlayerState.position = transform.GetPosition();
							hostData.nextPlayerState.orientation = transform.GetRotationEuler();
							hostData.nextPlayerState.velocity = rb.GetVelocity();
							hostData.nextPlayerState.angularVelocity = rb.GetAngularVelocity();
						}
					}
				}

				// sync the client frame index with the server
				const double currentTime = Time::Get().ElapsedSinceStartup();
				if( currentTime - hostConnection.lastSync > 3.f )
				{
					int max = hostConnection.framesDelta[0];
					int min = hostConnection.framesDelta[0];
					for( int i = 1; i < hostConnection.framesDelta.size(); i++ )
					{
						max = std::max( max, hostConnection.framesDelta[i] );
						min = std::min( min, hostConnection.framesDelta[i] );
					}

					if( max - min <= 1 ) // we have consistent readings
					{
						if( std::abs( min + hostManager.targetFrameDifference ) > 2 ) // only sync when we have a big enough frame index difference
						{
							Signal<>& success = hostReplication.Replicate(
								ClientRPC::RPCShiftClientFrame( min + hostManager.targetFrameDifference )
								, HostReplication::ResendUntilReplicated
							);
							hostConnection.lastSync = currentTime;
							success.Connect( &HostConnection::OnSyncSuccess, &hostConnection );

							Debug::Log() << "shifting host frame index : " << min + hostManager.targetFrameDifference;
							Debug::Get() << " " << hostConnection.ip.toString() << "::" << hostConnection.port << Debug::Endl();
						}
					}
				}
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Signature S_ServerNetworkSend::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<HostConnection>()  |
			_world.GetSignature<HostGameData>()    |
			_world.GetSignature<HostReplication>() |
			_world.GetSignature<ReliabilityLayer>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ServerNetworkSend::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		const HostManager&	hostManager = _world.GetSingletonComponent<HostManager>();
		const Game&			game		 = _world.GetSingletonComponent<Game>();
		ServerConnection&	connection  = _world.GetSingletonComponent<ServerConnection>();

		for( EntityID entityID : _entities )
		{
			HostConnection&		hostConnection	 = _world.GetComponent< HostConnection >( entityID );
			HostGameData&		hostData		 = _world.GetComponent< HostGameData >( entityID );
			HostReplication&	hostReplication	 = _world.GetComponent< HostReplication >( entityID );
			ReliabilityLayer&	reliabilityLayer = _world.GetComponent< ReliabilityLayer >( entityID );

			// create new packet			
			Packet packet( reliabilityLayer.GetNextPacketTag() );

			// write game data
			if( hostData.spaceshipID != 0 )
			{
				if( hostData.nextPlayerState.frameIndex == game.frameIndex )
				{
					hostData.nextPlayerState.Write( packet );
				}
				else
				{
					Debug::Warning() << "wrong host state frame index " << hostConnection.ip.toString() << "::" << hostConnection.port << Debug::Endl();
				}
			}

			hostConnection.Write( _world, packet );
			hostReplication.Write( packet );

			// write ack
			if( packet.GetSize() == sizeof( PacketTag ) ) { packet.onlyContainsAck = true; }
			reliabilityLayer.Write( packet );

			// send packet
			if( packet.GetSize() > sizeof( PacketTag ) )// don't send empty packets
			{
				reliabilityLayer.RegisterPacket( packet );
				hostConnection.bandwidth = 1.f / game.logicDelta * float( packet.GetSize() ) / 1000.f; // in Ko/s
				connection.socket.Send( packet, hostConnection.ip, hostConnection.port );
			}
			else
			{
				reliabilityLayer.nextPacketTag--;
			}
		}
	}
}