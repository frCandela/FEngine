#include "ecs/fanEcsSystem.hpp"
#include "network/components/fanClientReplication.hpp"
#include "network/components/fanClientConnection.hpp"
#include "network/components/fanClientGameData.hpp"
#include "network/components/fanClientRPC.hpp"
#include "network/singletons/fanTime.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "network/singletons/fanLinkingContext.hpp"

namespace fan
{
	//========================================================================================================
	// Save the player state ( transform & rigidbody ) for detecting desync with the server simulation
	//========================================================================================================
	struct S_ClientSaveState : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<ClientGameData>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			const Time& time = _world.GetSingleton<Time>();

			for( auto gameDataIt = _view.begin<ClientGameData>(); gameDataIt != _view.end<ClientGameData>(); ++gameDataIt )
			{
				ClientGameData& gameData = *gameDataIt;

				if( gameData.sSpaceshipHandle != 0 && gameData.mFrameSynced )
				{
					const EcsEntity spaceshipID = _world.GetEntity( gameData.sSpaceshipHandle );

					// saves previous player state
					const Rigidbody& rb = _world.GetComponent<Rigidbody>( spaceshipID );
					const Transform& transform = _world.GetComponent<Transform>( spaceshipID );
					assert( rb.mRigidbody->getTotalForce().isZero() );
					PacketPlayerGameState playerState;
					playerState.frameIndex = time.mFrameIndex;
					playerState.position = transform.GetPosition();
					playerState.orientation = transform.GetRotationEuler();
					playerState.velocity = rb.GetVelocity();
					playerState.angularVelocity = rb.GetAngularVelocity();
					gameData.mPreviousLocalStates.push( playerState );
				}
			}
		}
	};

	//========================================================================================================
	// Save the player input for sending over the network and making rollbacks in case of desync
	//========================================================================================================
	struct S_ClientSaveInput : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<ClientGameData>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			const Time& time = _world.GetSingleton<Time>();

			for( auto dataIt = _view.begin<ClientGameData>(); dataIt != _view.end<ClientGameData>(); ++dataIt)
			{
				ClientGameData& gameData = *dataIt;

				if( gameData.sSpaceshipHandle != 0 && gameData.mFrameSynced )
				{
					const EcsEntity spaceshipID = _world.GetEntity( gameData.sSpaceshipHandle );
					const PlayerInput& input = _world.GetComponent<PlayerInput>( spaceshipID );

					// streams input to the server
					PacketInput::InputData inputData;
					inputData.frameIndex = time.mFrameIndex;
					inputData.orientation = sf::Vector2f( input.mOrientation.x(), input.mOrientation.z() );
					inputData.left = input.mLeft > 0;
					inputData.right = input.mLeft < 0;
					inputData.forward = input.mForward > 0;
					inputData.backward = input.mForward < 0;
					inputData.boost = input.mBoost > 0;
					inputData.fire = input.mFire > 0;
					gameData.mPreviousInputs.push_front( inputData );
				}
			}
		}
	};

	//========================================================================================================
	// Replicates components, singleton components & runs RPC
	//========================================================================================================
	struct S_ClientRunReplication : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<ClientReplication>() | _world.GetSignature<ClientRPC>();
		}
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			LinkingContext& linkingContext = _world.GetSingleton<LinkingContext>();

			auto rpcIt = _view.begin<ClientRPC>();
			auto replicationIt = _view.begin<ClientReplication>();
			for( ; rpcIt != _view.end<ClientRPC>(); ++rpcIt, ++replicationIt )
			{
				ClientRPC& rpc = *rpcIt;
				ClientReplication& replication = *replicationIt;

				// replicate singletons components
				for( PacketReplication packet : replication.mReplicationListSingletons )
				{
					sf::Uint32 staticIndex;
					packet.packetData >> staticIndex;
					EcsSingleton& singleton = _world.GetSingleton( staticIndex );
					const EcsSingletonInfo& info = _world.GetSingletonInfo( staticIndex );
					info.netLoad( singleton, packet.packetData );
					assert( packet.packetData.endOfPacket() );
				}
				replication.mReplicationListSingletons.clear();

				// replicate entities
				for( PacketReplication packet : replication.mReplicationListEntities )
				{
					NetID netID;
					sf::Uint8 numComponents;

					packet.packetData >> netID;
					packet.packetData >> numComponents;

					auto it = linkingContext.mNetIDToEcsHandle.find( netID );
					if( it != linkingContext.mNetIDToEcsHandle.end() )
					{
						const EcsHandle replicatedHandle = it->second;
						const EcsEntity replicatedID = _world.GetEntity( replicatedHandle );
						if( _world.HasComponent<SceneNode>( replicatedID ) )
						{
                            SceneNode& node = _world.GetComponent<SceneNode>( replicatedID );
                            node.AddFlag( SceneNode::Flags::BoundsOutdated );
						}


						for( int i = 0; i < numComponents; i++ )
						{
							sf::Uint32 staticIndex;
							packet.packetData >> staticIndex;
							const EcsComponentInfo& info = _world.GetComponentInfo( staticIndex );
							EcsComponent& component = _world.GetComponent( replicatedID, staticIndex );
							info.netLoad( component, packet.packetData );
						}
					}
				}
				replication.mReplicationListEntities.clear();

				// replicate RPC
				for( PacketReplication packet : replication.mReplicationListRPC )
				{
					rpc.TriggerRPC( packet.packetData );
				}
				replication.mReplicationListRPC.clear();
			}
		}
	};

	//========================================================================================================
	// Detect server timeout on all clients connections
	//========================================================================================================
	struct S_ClientDetectServerTimeout : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<ClientConnection>();
		}
		static void Run( EcsWorld& /*_world*/, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

            for( auto connectionIt = _view.begin<ClientConnection>();
                 connectionIt != _view.end<ClientConnection>();
                 ++connectionIt )
			{
				ClientConnection& connection = *connectionIt;

				if( connection.mState == ClientConnection::ClientState::Connected )
				{
					const double currentTime = Time::ElapsedSinceStartup();
					if( connection.mServerLastResponse + connection.mTimeoutDelay < currentTime )
					{
						Debug::Log() << "server timeout " << Debug::Endl();
						connection.mState = ClientConnection::ClientState::Disconnected;
					}
				}
			}
		}
	};
}