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
#include "network/singletonComponents/fanClientReplication.hpp"
#include "network/singletonComponents/fanClientConnection.hpp"
#include "network/singletonComponents/fanLinkingContext.hpp"
#include "network/singletonComponents/fanRPCManager.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "network/systems/fanTimeout.hpp"

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
		netManager.spaceshipSpawnFrameIndex = 0;
		netManager.spaceshipNetID = 0;
		netManager.spaceshipHandle = 0;
		netManager.persistentHandle = 0;
		netManager.synced = false;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Start( EcsWorld& _world )
	{
		// connect rpc
		Game& game = _world.GetSingletonComponent<Game>();
		RPCManager& rpcManager = _world.GetSingletonComponent<RPCManager>();
		rpcManager.onShiftFrameIndex.Connect( &ClientNetworkManager::OnShiftFrameIndex, this );
		rpcManager.onShiftFrameIndex.Connect( &Game::OnShiftFrameIndex, &game );
		rpcManager.onSpawnShip.Connect( &ClientNetworkManager::OnSpawnShip, this );

		// Create player persistent scene node
		Scene& scene			= _world.GetSingletonComponent<Scene>();
		SceneNode& sceneNode	= scene.CreateSceneNode( "persistent", scene.root );
		persistentHandle		= sceneNode.handle;
		EntityID entityID		= _world.GetEntityID( persistentHandle );
		_world.AddComponent<ReliabilityLayer>( entityID );

		// Bind socket
		ClientConnection& connection = _world.GetSingletonComponent<ClientConnection>();
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
		ClientConnection& connection = _world.GetSingletonComponent<ClientConnection>();
		connection.state = ClientConnection::ClientState::Stopping;
		NetworkSend( _world ); // send a last packet
		connection.socket.Unbind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::OnSpawnShip( NetID _spaceshipID, FrameIndex _frameIndex )
	{
		if( spaceshipNetID == 0 )
		{
			spaceshipSpawnFrameIndex = _frameIndex;
			spaceshipNetID			 = _spaceshipID;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::OnShiftFrameIndex( const int _framesDelta )
	{
		previousStates = std::queue< PacketPlayerGameState >(); // clear
		synced = true;
		Debug::Log() << "Shifted client frame index : " << _framesDelta << Debug::Endl();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::ProcessPacket( const PacketPlayerGameState& _packet )
	{
		// get the current input for this client
		while( ! previousStates.empty() )
		{
			const PacketPlayerGameState& packetInput = previousStates.front();
			if( packetInput.frameIndex < _packet.frameIndex )
			{
				previousStates.pop();
			}
			else
			{
				break;
			}
		}

		// moves spaceship						
		if( !previousStates.empty() && previousStates.front().frameIndex == _packet.frameIndex )
		{
			const PacketPlayerGameState& packetState = previousStates.front();
			previousStates.pop();

			if( packetState != _packet )
			{
				Debug::Warning() << "player is out of sync" << Debug::Endl();
			}

		}
		else
		{
			Debug::Warning() << "no available game state for this frame" << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Update( EcsWorld& _world )
	{
		ClientReplication& replication	= _world.GetSingletonComponent<ClientReplication>();
		LinkingContext& linkingContext  = _world.GetSingletonComponent<LinkingContext>();
		Game& game						= _world.GetSingletonComponent<Game>();
		ClientConnection& connection	= _world.GetSingletonComponent<ClientConnection>();

		replication.ReplicateSingletons( _world );

		// spawns spaceship
		if( spaceshipSpawnFrameIndex != 0 && game.frameIndex >= spaceshipSpawnFrameIndex )
		{
			assert( spaceshipNetID != 0 );

			spaceshipSpawnFrameIndex = 0;
			spaceshipHandle = Game::SpawnSpaceship( _world );
			linkingContext.AddEntity( spaceshipHandle, spaceshipNetID );

			const EntityID spaceshipID = _world.GetEntityID( spaceshipHandle );
			if( spaceshipHandle != 0 )
			{
				_world.AddComponent<PlayerController>( spaceshipID );
			}
		}

		if( spaceshipHandle != 0  && synced )
		{
			const EntityID entityID = _world.GetEntityID( spaceshipHandle );
			const PlayerInput& input = _world.GetComponent<PlayerInput>( entityID );

			// streams input to the server
			PacketInput packetInput;
			packetInput.frameIndex = game.frameIndex;
			packetInput.orientation = input.orientation;
			packetInput.left = input.left;
			packetInput.forward = input.forward;
			packetInput.boost = input.boost;
			packetInput.fire = input.fire;
			inputs.push( packetInput );

			// saves previous player state
			const Rigidbody& rb = _world.GetComponent<Rigidbody>( entityID );
			const Transform& transform = _world.GetComponent<Transform>( entityID );
			PacketPlayerGameState playerState;			
			playerState.frameIndex = game.frameIndex;
			playerState.position = transform.GetPosition();
			playerState.orientation = transform.GetRotationEuler();
			playerState.velocity = rb.GetVelocity();
			playerState.angularVelocity = rb.GetAngularVelocity();
			previousStates.push( playerState );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::NetworkReceive( EcsWorld& _world )
	{
		S_ProcessTimedOutPackets::Run( _world, _world.Match( S_ProcessTimedOutPackets::GetSignature( _world ) ) );

		const EntityID presistentEntityID = _world.GetEntityID( persistentHandle );
		ReliabilityLayer& reliabilityLayer = _world.GetComponent<ReliabilityLayer>( presistentEntityID );
		ClientConnection& connection = _world.GetSingletonComponent<ClientConnection>();
		ClientReplication& replication = _world.GetSingletonComponent<ClientReplication>();
		RPCManager& rpcManager = _world.GetSingletonComponent<RPCManager>();
		Game& game = _world.GetSingletonComponent<Game>();

		// receive
		Packet			packet;
		sf::IpAddress	receiveIP;
		unsigned short	receivePort;

		sf::Socket::Status socketStatus;
		do
		{
			packet.Clear();
			socketStatus = connection.socket.Receive( packet, receiveIP, receivePort );

			// only receive from the server
			if( receiveIP != connection.serverIP || receivePort != connection.serverPort )
			{
				continue;
			}

			switch( socketStatus )
			{
			case sf::UdpSocket::Done:
			{
				connection.serverLastResponse = Time::Get().ElapsedSinceStartup();

				// read the first packet type separately
				PacketType packetType = packet.ReadType();
				if( packetType == PacketType::Ack )
				{
					packet.onlyContainsAck = true;
				}				
				else if( packetType == PacketType::Disconnect )
				{
					// disconnection can cause the reliability layer tags to be off
					reliabilityLayer.expectedPacketTag = packet.tag;
				}

				if( !reliabilityLayer.ValidatePacket( packet ) )
				{
					continue;
				}

				// process packet
				bool packetValid = true;
				while( packetValid )
				{
					switch( packetType )
					{
					case PacketType::Ack:
					{
						PacketAck packetAck;
						packetAck.Read( packet );
						reliabilityLayer.ProcessPacket( packetAck );
					}break;
					case PacketType::Ping:
					{
						PacketPing packetPing;
						packetPing.Read( packet );
						connection.ProcessPacket( packetPing, game.frameIndex );
					} break;
					case PacketType::LoggedIn:
					{
						PacketLoginSuccess packetLogin;
						packetLogin.Read( packet );
						connection.ProcessPacket( packetLogin );
					} break;
					case PacketType::Disconnect:
					{
						PacketDisconnect packetDisconnect;
						packetDisconnect.Read( packet );
						connection.ProcessPacket( packetDisconnect );
					} break;

					case PacketType::Replication:
					{
						PacketReplication packetReplication;
						packetReplication.Read( packet );
						replication.ProcessPacket( packetReplication );
					} break;
					case PacketType::PlayerGameState:
					{
						PacketPlayerGameState packetPlayerGameState;
						packetPlayerGameState.Read( packet );
						ProcessPacket( packetPlayerGameState );
					} break;
					default:
						Debug::Warning() << "Invalid packet " << int( packetType ) << " received. Reading canceled." << Debug::Endl();
						packetValid = false;
						break;
					}

					// stop if we reach the end or reads the next packet type
					if( packet.EndOfPacket() )
					{
						break;
					}
					else
					{
						packetType = packet.ReadType();
					}
				}
			} break;
			case sf::UdpSocket::Error:
				Debug::Warning() << "socket.receive: an unexpected error happened " << Debug::Endl();
				break;
			case sf::UdpSocket::Partial:
			case sf::UdpSocket::NotReady:
			{
				// do nothing
			}break;
			case sf::UdpSocket::Disconnected:
			{
				// disconnect
			} break;
			default:
				assert( false );
				break;
			}
		} while( socketStatus == sf::UdpSocket::Done );

		
		connection.DetectServerTimout();		
		replication.ReplicateRPC( rpcManager );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::NetworkSend( EcsWorld& _world )
	{
		const EntityID presistentEntityID = _world.GetEntityID( persistentHandle );
		ReliabilityLayer& reliabilityLayer = _world.GetComponent<ReliabilityLayer>( presistentEntityID );
		ClientConnection& connection = _world.GetSingletonComponent<ClientConnection>();
		Game& game = _world.GetSingletonComponent<Game>();

		// create packet
		Packet packet( reliabilityLayer.GetNextPacketTag() );

		// write packet
		connection.Write( packet );

		if( !inputs.empty() )
		{
			const PacketInput& lastInput = inputs.front();
			inputs.pop();
			assert( lastInput.frameIndex == game.frameIndex );
			lastInput.Write( packet );
		}

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