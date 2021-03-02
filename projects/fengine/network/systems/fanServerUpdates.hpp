#include "core/ecs/fanEcsSystem.hpp"
#include "network/components/fanHostConnection.hpp"
#include "network/components/fanHostReplication.hpp"
#include "network/components/fanHostGameData.hpp"
#include "network/singletons/fanTime.hpp"
#include "network/singletons/fanHostManager.hpp"
#include "network/systems/fanHostReplication.hpp"
#include "game/spawn/fanSpawnShip.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/singletons/fanSolarEruption.hpp"
#include "game/spawn/fanSpawnSolarEruption.hpp"

namespace fan
{
	//========================================================================================================
	// Runs the spawn ship RPC on synchronized hosts
	//========================================================================================================
	struct SHostSpawnShip : EcsSystem
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

			LinkingContext& linkingContext = _world.GetSingleton<LinkingContext>();
			SpawnManager& spawnManager = _world.GetSingleton<SpawnManager>();
			HostManager& hostManager = _world.GetSingleton<HostManager>();
			const Time& time = _world.GetSingleton<Time>();

			auto hostConnectionIt = _view.begin<HostConnection>();
			auto hostDataIt = _view.begin<HostGameData>();
			auto hostReplicationIt = _view.begin<HostReplication>();
            for( ; hostConnectionIt != _view.end<HostConnection>();
                   ++hostConnectionIt, ++hostDataIt, ++hostReplicationIt )
			{
				const EcsHandle hostHandle = _world.GetHandle( hostConnectionIt.GetEntity() );
				const HostConnection& hostConnection = *hostConnectionIt;
				HostReplication& hostReplication = *hostReplicationIt;
				HostGameData& hostData = *hostDataIt;

                if( hostConnection.mState == HostConnection::Connected &&
                    hostConnection.mSynced &&
                    hostData.mSpaceshipID == 0 )
				{
					// spawns new host spaceship
					hostData.mSpaceshipID = linkingContext.mNextNetID++;	// assigns net id for the new ship
					const FrameIndex spawnFrame = time.mFrameIndex + 60;
                    const SpawnInfo spawnInfo = spawn::SpawnShip::GenerateInfo( hostHandle,
                                                                                spawnFrame,
                                                                                hostData.mSpaceshipID,
                                                                                btVector3_Zero );
					hostData.mNextPlayerStateFrame = spawnFrame + 60; // timing of the first state snapshot
					spawnManager.spawns.push_back( spawnInfo );		  // triggers spaceship spawn on server
					
					// spawn new ship on all hosts
                    _world.Run<SReplicateOnAllHosts>( ClientRPC::RPCSpawn( spawnInfo ),
                                                      HostReplication::ResendUntilReplicated );

					// replicate all other hosts on new host
					for( const auto& pair : hostManager.mHostHandles )
					{
						// do not replicate  itself
						const EcsHandle otherHostHandle = pair.second;
						if( otherHostHandle == hostHandle )
						{
							continue;
						}	

						// replicate all other ships that are already spawned
						EcsEntity otherHostEntity = _world.GetEntity( otherHostHandle );
						HostGameData& otherHostData = _world.GetComponent< HostGameData >( otherHostEntity );
						if( otherHostData.mSpaceshipID != 0 )
						{
                            const SpawnInfo otherHostspawnInfo = spawn::SpawnShip::GenerateInfo(
                                    otherHostHandle,
                                    spawnFrame,
                                    otherHostData.mSpaceshipID,
                                    btVector3_Zero );
							hostReplication.Replicate(
								ClientRPC::RPCSpawn( otherHostspawnInfo )
								, HostReplication::ResendUntilReplicated
							);
						}						
					}

					// replicates solar eruption spawn
					const SolarEruption& solarEruption = _world.GetSingleton<SolarEruption>();
					const SpawnInfo eruptionSpawnInfo = spawn::SpawnSolarEruption::GenerateInfo( solarEruption.mSpawnFrame );
					hostReplication.Replicate(
						ClientRPC::RPCSpawn( eruptionSpawnInfo )
						, HostReplication::ResendUntilReplicated
					);
				}
			}
		}
	};

	//========================================================================================================
	// When host frame index is offset too far from the server frame index, syncs it brutally
	//========================================================================================================
	struct SHostSyncFrame : EcsSystem
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
				if( currentTime - hostConnection.mLastSync > 1.f )
				{
					int max = hostConnection.mFramesDelta[0];
					int min = hostConnection.mFramesDelta[0];
					for( int i = 1; i < (int)hostConnection.mFramesDelta.size(); i++ )
					{
						max = std::max( max, hostConnection.mFramesDelta[i] );
						min = std::min( min, hostConnection.mFramesDelta[i] );
					}

					if( max - min <= 1 ) // we have consistent readings
					{
						// @todo calculate buffer size depending on jitter, not rtt
                        hostConnection.mTargetBufferSize = int( 1000.f *
                                                                hostConnection.mRtt *
                                                                ( 5.f - 2.f )
                                                                / 100.f
                                                                + 2.f ); // size 5 at 100 ms rtt
						hostConnection.mTargetBufferSize = std::min( hostConnection.mTargetBufferSize, 15 );

                        const int targetFrameDifference = int( hostConnection.mRtt /
                                                               2.f /
                                                               time.mLogicDelta ) +
                                                          hostConnection.mTargetBufferSize;
						const int diff = std::abs( min + targetFrameDifference );
						if( diff > 1 ) // only sync when we have a big enough frame index difference
						{
							Signal<>& success = hostReplication.Replicate(
								ClientRPC::RPCShiftClientFrame( min + targetFrameDifference )
								, HostReplication::ResendUntilReplicated
							);
							hostConnection.mLastSync = currentTime;
                            success.Connect( &HostConnection::OnSyncSuccess,
                                             _world,
                                             _world.GetHandle( hostConnectionIt.GetEntity() ) );
							if( diff > 10 )
							{
								Debug::Log() << "shifting host frame index : " << min + targetFrameDifference;
								Debug::Get() << " " << hostConnection.mIp.toString()
								             << "::" << hostConnection.mPort << Debug::Endl();
							}
						}
					}
				}
			}
		}
	};

	//========================================================================================================
	// Save the host state ( transform & rigidbody ) for detecting desync with the host simulation
	//========================================================================================================
	struct SHostSaveState : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<HostGameData>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			const Time& time = _world.GetSingleton<Time>();

            for( auto hostDataIt = _view.begin<HostGameData>();
                 hostDataIt != _view.end<HostGameData>();
                 ++hostDataIt )
			{
				HostGameData& hostData = *hostDataIt;

				if( hostData.mSpaceshipHandle != 0 && time.mFrameIndex >= hostData.mNextPlayerStateFrame )
				{
					const EcsEntity shipEntityID = _world.GetEntity( hostData.mSpaceshipHandle );
					const Rigidbody& rb = _world.GetComponent<Rigidbody>( shipEntityID );
					const Transform& transform = _world.GetComponent<Transform>( shipEntityID );
					hostData.mNextPlayerState.mFrameIndex      = time.mFrameIndex;
					hostData.mNextPlayerState.mPosition        = transform.GetPosition();
					hostData.mNextPlayerState.mOrientation     = transform.GetRotationEuler();
					hostData.mNextPlayerState.mVelocity        = rb.GetVelocity();
					hostData.mNextPlayerState.mAngularVelocity = rb.GetAngularVelocity();

					hostData.mNextPlayerStateFrame = time.mFrameIndex + 7;
				}
			}
		}
	};

	//========================================================================================================
	// Updates the host input with the one received over network
	//========================================================================================================
	struct SHostUpdateInput : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<HostGameData>();
		}
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			const Time& time = _world.GetSingleton<Time>();

            for( auto hostDataIt = _view.begin<HostGameData>();
                 hostDataIt != _view.end<HostGameData>();
                 ++hostDataIt )
			{
				HostGameData& hostData = *hostDataIt;

				if( hostData.mSpaceshipID != 0 )
				{
					// Get the current input (current frame) for this client
					while( !hostData.mInputs.empty() )
					{
						const PacketInput::InputData& inputData = hostData.mInputs.front();
                        if( inputData.mFrameIndex < time.mFrameIndex ||
                            inputData.mFrameIndex > time.mFrameIndex + 60 )
						{
							hostData.mInputs.pop();
						}
						else
						{
							break;
						}
					}

					if( hostData.mSpaceshipHandle != 0 )
					{
						// Updates spaceship input
                        if( !hostData.mInputs.empty() &&
                            hostData.mInputs.front().mFrameIndex == time.mFrameIndex )
						{
							const PacketInput::InputData& inputData = hostData.mInputs.front();
							hostData.mInputs.pop();

							const EcsEntity shipEntityID = _world.GetEntity( hostData.mSpaceshipHandle );
							PlayerInput& input = _world.GetComponent<PlayerInput>( shipEntityID );
                            input.mOrientation = btVector3( inputData.mOrientation.x,
                                                            0.f,
                                                            inputData.mOrientation.y );
							input.mLeft        = inputData.mLeft ? 1.f : ( inputData.mRight ? -1.f : 0.f );
							input.mForward     = inputData.mForward ? 1.f : ( inputData.mBackward ? -1.f : 0.f );
							input.mBoost       = inputData.mBoost;
							input.mFire        = inputData.mFire;
						}
					}
				}
			}
		}
	};
}