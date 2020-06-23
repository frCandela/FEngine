#include "ecs/fanEcsSystem.hpp"
#include "network/components/fanHostConnection.hpp"
#include "network/components/fanHostReplication.hpp"
#include "network/components/fanHostGameData.hpp"
#include "game/spawn/fanSpawnShip.hpp"
#include "network/singletons/fanTime.hpp"
#include "game/components/fanPlayerInput.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// Runs the spawn ship RPC on synchronized hosts
	//==============================================================================================================================================================
	struct S_HostSpawnShip : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<HostConnection>() |
				_world.GetSignature<HostGameData>() |
				_world.GetSignature<HostReplication>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			HostManager& hostManager = _world.GetSingleton<HostManager>();
			LinkingContext& linkingContext = _world.GetSingleton<LinkingContext>();
			const Time& time = _world.GetSingleton<Time>();

			auto hostConnectionIt = _view.begin<HostConnection>();
			auto hostDataIt = _view.begin<HostGameData>();
			auto hostReplicationIt = _view.begin<HostReplication>();
			for( ; hostConnectionIt != _view.end<HostConnection>(); ++hostConnectionIt, ++hostDataIt, ++hostReplicationIt )
			{
				const EcsEntity entity = hostConnectionIt.GetEntity();
				const HostConnection& hostConnection = *hostConnectionIt;
				HostGameData& hostData = *hostDataIt;
				HostReplication& hostReplication = *hostReplicationIt;

				if( hostConnection.state == HostConnection::Connected )
				{
					if( hostConnection.synced == true )
					{
						if( hostData.spaceshipID == 0 )
						{
							// spawns new host spaceship
							hostData.spaceshipID = linkingContext.nextNetID++;
							hostData.spaceshipHandle = Game::SpawnSpaceship( _world, true, false );
							linkingContext.AddEntity( hostData.spaceshipHandle, hostData.spaceshipID );
							const FrameIndex spawnFrame = time.frameIndex + 60;
							const SpawnInfo spawnInfo0 = spawn::SpawnClientShip::GenerateInfo( spawnFrame, hostData.spaceshipID, btVector3::Zero() );
							hostReplication.Replicate(
								ClientRPC::RPCSpawn( spawnInfo0 )
								, HostReplication::ResendUntilReplicated
							);

							hostData.nextPlayerStateFrame = spawnFrame + 60; // first player state snapshot will be done later to let time for input to arrive

							// replicate other ships
							for( const auto& pair : hostManager.hostHandles )
							{
								const EcsHandle hostHandle = _world.GetHandle( entity );
								const EcsHandle otherHostHandle = pair.second;

								if( otherHostHandle != hostHandle )
								{
									const EcsEntity otherHostEntity = _world.GetEntity( otherHostHandle );

									// replicate new host on all other hosts
									const SpawnInfo spawnInfo1 = spawn::SpawnShip::GenerateInfo( spawnFrame, hostData.spaceshipID, btVector3::Zero() );
									HostReplication& otherHostReplication = _world.GetComponent< HostReplication >( otherHostEntity );
									otherHostReplication.Replicate(
										ClientRPC::RPCSpawn( spawnInfo1 )
										, HostReplication::ResendUntilReplicated
									);

									// replicate all other hosts on new host		
									HostGameData& otherHostData = _world.GetComponent< HostGameData >( otherHostEntity );
									const SpawnInfo spawnInfo2 = spawn::SpawnShip::GenerateInfo( spawnFrame, otherHostData.spaceshipID, btVector3::Zero() );
									hostReplication.Replicate(
										ClientRPC::RPCSpawn( spawnInfo2 )
										, HostReplication::ResendUntilReplicated
									);
								}
							}
						}
					}
				}
			}
		}
	};

	//==============================================================================================================================================================
	// When host frame index is offset too far from the server frame index, syncs it brutally
	//==============================================================================================================================================================
	struct S_HostSyncFrame : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<HostConnection>() |
				_world.GetSignature<HostReplication>();
		}
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			const Time& time = _world.GetSingleton<Time>();

			auto hostConnectionIt = _view.begin<HostConnection>();
			auto hostReplicationIt = _view.begin<HostReplication>();
			for( ; hostConnectionIt != _view.end<HostConnection>(); ++hostConnectionIt, ++hostReplicationIt )
			{
				HostConnection& hostConnection = *hostConnectionIt;
				HostReplication& hostReplication = *hostReplicationIt;

				// sync the client frame index with the server
				const double currentTime = Time::ElapsedSinceStartup();
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
						hostConnection.targetBufferSize = int( 1000.f * hostConnection.rtt * ( 5.f - 2.f ) / 100.f + 2.f ); // size 5 at 100 ms rtt
						hostConnection.targetBufferSize = std::min( hostConnection.targetBufferSize, 15 );

						const int targetFrameDifference = int( hostConnection.rtt / 2.f / time.logicDelta ) + hostConnection.targetBufferSize;
						const int diff = std::abs( min + targetFrameDifference );
						if( diff > 1 ) // only sync when we have a big enough frame index difference
						{
							Signal<>& success = hostReplication.Replicate(
								ClientRPC::RPCShiftClientFrame( min + targetFrameDifference )
								, HostReplication::ResendUntilReplicated
							);
							hostConnection.lastSync = currentTime;
							success.Connect( &HostConnection::OnSyncSuccess, _world, _world.GetHandle( hostConnectionIt.GetEntity() ) );
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
	};

	//==============================================================================================================================================================
	// Save the host state ( transform & rigidbody ) for detecting desync with the host simulation
	//==============================================================================================================================================================
	struct S_HostSaveState : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<HostGameData>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			const Time& time = _world.GetSingleton<Time>();

			for( auto hostDataIt = _view.begin<HostGameData>(); hostDataIt != _view.end<HostGameData>(); ++hostDataIt )
			{
				HostGameData& hostData = *hostDataIt;

				if( hostData.spaceshipHandle != 0 && time.frameIndex >= hostData.nextPlayerStateFrame )
				{
					const EcsEntity shipEntityID = _world.GetEntity( hostData.spaceshipHandle );
					const Rigidbody& rb = _world.GetComponent<Rigidbody>( shipEntityID );
					const Transform& transform = _world.GetComponent<Transform>( shipEntityID );
					hostData.nextPlayerState.frameIndex = time.frameIndex;
					hostData.nextPlayerState.position = transform.GetPosition();
					hostData.nextPlayerState.orientation = transform.GetRotationEuler();
					hostData.nextPlayerState.velocity = rb.GetVelocity();
					hostData.nextPlayerState.angularVelocity = rb.GetAngularVelocity();

					hostData.nextPlayerStateFrame = time.frameIndex + 7;
				}
			}
		}
	};

	//==============================================================================================================================================================
	// Updates the host input with the one received over network
	//==============================================================================================================================================================
	struct S_HostUpdateInput : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<HostGameData>();
		}
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			const Time& time = _world.GetSingleton<Time>();

			for( auto hostDataIt = _view.begin<HostGameData>(); hostDataIt != _view.end<HostGameData>(); ++hostDataIt )
			{
				HostGameData& hostData = *hostDataIt;

				if( hostData.spaceshipID != 0 )
				{
					// Get the current input (current frame) for this client
					while( !hostData.inputs.empty() )
					{
						const PacketInput::InputData& inputData = hostData.inputs.front();
						if( inputData.frameIndex < time.frameIndex || inputData.frameIndex > time.frameIndex + 60 )
						{
							hostData.inputs.pop();
						}
						else
						{
							break;
						}
					}

					// Updates spaceship input
					if( !hostData.inputs.empty() && hostData.inputs.front().frameIndex == time.frameIndex )
					{
						const PacketInput::InputData& inputData = hostData.inputs.front();
						hostData.inputs.pop();

						const EcsEntity shipEntityID = _world.GetEntity( hostData.spaceshipHandle );
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
	};
}