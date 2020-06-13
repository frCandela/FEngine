#include "network/systems/fanClientUpdates.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanTime.hpp"
#include "network/singletons/fanLinkingContext.hpp"
#include "network/components/fanClientReplication.hpp"
#include "network/components/fanClientConnection.hpp"
#include "network/components/fanClientGameData.hpp"
#include "network/components/fanClientRPC.hpp"
#include "game/singletons/fanGame.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/components/fanPlayerController.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"

namespace fan
{ 
	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_ClientSaveState::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<ClientGameData>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ClientSaveState::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		const Game& game = _world.GetSingleton<Game>();

		for( auto gameDataIt = _view.begin<ClientGameData>(); gameDataIt != _view.end<ClientGameData>(); ++gameDataIt )
		{
			ClientGameData& gameData = *gameDataIt;

			if( gameData.spaceshipHandle != 0 && gameData.frameSynced )
			{
				const EcsEntity spaceshipID = _world.GetEntity( gameData.spaceshipHandle );

				// saves previous player state
				const Rigidbody& rb = _world.GetComponent<Rigidbody>( spaceshipID );
				const Transform& transform = _world.GetComponent<Transform>( spaceshipID );
				assert( rb.rigidbody->getTotalForce().isZero() );
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
	void S_ClientRunReplication::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
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
			for( PacketReplication packet : replication.replicationListSingletons )
			{
				sf::Uint32 staticIndex;
				packet.packetData >> staticIndex;
				EcsSingleton& singleton = _world.GetSingleton( staticIndex );
				const EcsSingletonInfo& info = _world.GetSingletonInfo( staticIndex );
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
						const EcsComponentInfo& info = _world.GetComponentInfo( staticIndex );
						EcsComponent& component = _world.GetComponent( replicatedID, staticIndex );
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
	void S_ClientDetectServerTimeout::Run( EcsWorld& /*_world*/, const EcsView& _view, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		for( auto connectionIt = _view.begin<ClientConnection>(); connectionIt != _view.end<ClientConnection>(); ++connectionIt )
		{
			ClientConnection& connection = *connectionIt;
			
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
	void S_ClientSaveInput::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		const Game& game = _world.GetSingleton<Game>();

		for( auto gameDataIt = _view.begin<ClientGameData>(); gameDataIt != _view.end<ClientGameData>(); ++gameDataIt )
		{
			ClientGameData& gameData = *gameDataIt;

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