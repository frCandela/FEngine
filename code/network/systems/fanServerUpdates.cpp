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
	Signature S_HostSpawnShip::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<HostConnection>() |
			_world.GetSignature<HostGameData>()   |
			_world.GetSignature<HostReplication>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_HostSpawnShip::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		HostManager& hostManager = _world.GetSingletonComponent<HostManager>();
		LinkingContext& linkingContext = _world.GetSingletonComponent<LinkingContext>();
		const Game& game = _world.GetSingletonComponent<Game>();

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
						// spawns new host spaceship
						hostData.spaceshipHandle = Game::SpawnSpaceship( _world );
						hostData.spaceshipID = linkingContext.nextNetID++;
						linkingContext.AddEntity( hostData.spaceshipHandle, hostData.spaceshipID );
						const FrameIndexNet spawnFrame = game.frameIndex + 60;
						hostReplication.Replicate(
							ClientRPC::RPCSpawnClientShip( hostData.spaceshipID, spawnFrame )
							, HostReplication::ResendUntilReplicated
						);

						hostData.nextPlayerStateFrame = spawnFrame + 60; // next player state snapshot done later

						// replicate other ships
						for( const auto& pair : hostManager.hostHandles )
						{
							const EntityHandle otherHostHandle = pair.second;
							const EntityID otherHostID = _world.GetEntityID( otherHostHandle );

							if( otherHostID != entityID )
							{
								// replicate new host on all other hosts
								HostReplication& otherHostReplication = _world.GetComponent< HostReplication >( otherHostID );
								otherHostReplication.Replicate(
									ClientRPC::RPCSpawnShip( hostData.spaceshipID, spawnFrame )
									, HostReplication::ResendUntilReplicated
								);

 								// replicate all other hosts on new host		
								HostGameData& otherHostData = _world.GetComponent< HostGameData >( otherHostID );
								hostReplication.Replicate(
									ClientRPC::RPCSpawnShip( otherHostData.spaceshipID, game.frameIndex )
									, HostReplication::ResendUntilReplicated
								);
							}
						}
					}
				}
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Signature S_HostSyncFrame::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<HostConnection>() |
			_world.GetSignature<HostReplication>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_HostSyncFrame::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		const HostManager& hostManager = _world.GetSingletonComponent<HostManager>();
		const Game& game = _world.GetSingletonComponent<Game>();

		for( EntityID entityID : _entities )
		{
			HostConnection&		hostConnection  = _world.GetComponent< HostConnection >( entityID );
			HostReplication&	hostReplication = _world.GetComponent< HostReplication >( entityID );

			// sync the client frame index with the server
			const double currentTime = Time::Get().ElapsedSinceStartup();
			if( currentTime - hostConnection.lastSync > 1.f )
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
					// @todo calculate buffer size depending on jitter, not rtt
					hostConnection.targetBufferSize = int( 1000.f * hostConnection.rtt * (5.f - 2.f) / 100.f + 2.f ); // size 5 at 100 ms rtt
					hostConnection.targetBufferSize = std::min( hostConnection.targetBufferSize, 15 );
					
					const int targetFrameDifference =  int( hostConnection.rtt / 2.f / game.logicDelta ) + hostConnection.targetBufferSize;
					const int diff = std::abs( min + targetFrameDifference );
					if( diff > 1 ) // only sync when we have a big enough frame index difference
					{
						Signal<>& success = hostReplication.Replicate(
							ClientRPC::RPCShiftClientFrame( min + targetFrameDifference )
							, HostReplication::ResendUntilReplicated
						);
						hostConnection.lastSync = currentTime;
						success.Connect( &HostConnection::OnSyncSuccess, &hostConnection );

						if( diff > 10 )
						{
							Debug::Log() << "shifting host frame index : " << min + targetFrameDifference;
							Debug::Get() << " " << hostConnection.ip.toString() << "::" << hostConnection.port << Debug::Endl();
						}
					}
				}
			}
		}
	}	


	//================================================================================================================================
	//================================================================================================================================
	Signature S_HostSaveState::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<HostGameData>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_HostSaveState::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		const Game& game = _world.GetSingletonComponent<Game>();

		for( EntityID entityID : _entities )
		{
			HostGameData& hostData = _world.GetComponent< HostGameData >( entityID );

			if(  hostData.spaceshipHandle != 0 && game.frameIndex >= hostData.nextPlayerStateFrame )
			{
				const EntityID shipEntityID = _world.GetEntityID( hostData.spaceshipHandle );
				const Rigidbody& rb = _world.GetComponent<Rigidbody>( shipEntityID );
				const Transform& transform = _world.GetComponent<Transform>( shipEntityID );
				hostData.nextPlayerState.frameIndex = game.frameIndex;
				hostData.nextPlayerState.position = transform.GetPosition();
				hostData.nextPlayerState.orientation = transform.GetRotationEuler();
				hostData.nextPlayerState.velocity = rb.GetVelocity();
				hostData.nextPlayerState.angularVelocity = rb.GetAngularVelocity();

				hostData.nextPlayerStateFrame = game.frameIndex + 7;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Signature S_HostUpdateInput::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<HostGameData>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_HostUpdateInput::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		const Game& game = _world.GetSingletonComponent<Game>();

		for( EntityID entityID : _entities )
		{
			HostGameData& hostData = _world.GetComponent< HostGameData >( entityID );

			if( hostData.spaceshipID != 0 )
			{
				// Get the current input (current frame) for this client
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

				// Updates spaceship input
				if( !hostData.inputs.empty() && hostData.inputs.front().frameIndex == game.frameIndex )
				{
					const PacketInput::InputData& inputData = hostData.inputs.front();
					hostData.inputs.pop();

					const EntityID shipEntityID = _world.GetEntityID( hostData.spaceshipHandle );
					PlayerInput& input = _world.GetComponent<PlayerInput>( shipEntityID );
					input.orientation = btVector3( inputData.orientation.x, 0.f, inputData.orientation.y );
					input.left = inputData.left ? 1.f : ( inputData.right ? -1.f : 0.f );
					input.forward = inputData.forward ? 1.f : ( inputData.backward ? -1.f : 0.f );
					input.boost = inputData.boost;
					input.fire = inputData.fire;
				}
				else
				{
					Debug::Warning() << "no available input from player " << Debug::Endl();
				}
			}
		}
	}	
}