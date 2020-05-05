#include "game/singletonComponents/fanClientNetworkManager.hpp"

#include "core/fanDebug.hpp"
#include "scene/fanSceneSerializable.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanTime.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "game/components/fanPlayerController.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "network/singletonComponents/fanDeliveryNotificationManager.hpp"
#include "network/singletonComponents/fanClientReplicationManager.hpp"
#include "network/singletonComponents/fanClientConnectionManager.hpp"
#include "network/singletonComponents/fanLinkingContext.hpp"
#include "network/singletonComponents/fanRPCManager.hpp"

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
		_info.save = &ClientNetworkManager::Save;
		_info.load = &ClientNetworkManager::Load;
		_info.name = "client network manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		ClientNetworkManager& netManager = static_cast<ClientNetworkManager&>( _component );
		netManager.deliveryNotificationManager = nullptr;
		netManager.replicationManager = nullptr;
		netManager.linkingContext = nullptr;
		netManager.connection = nullptr;
		netManager.rpcManager = nullptr;
		netManager.game = nullptr;
		netManager.spaceshipSpawnFrameIndex = 0;
		netManager.spaceshipNetID = 0;
		netManager.spaceshipHandle = 0;
		netManager.synced = false;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Start( EcsWorld& _world )
	{
		deliveryNotificationManager = &_world.GetSingletonComponent<DeliveryNotificationManager>();
		replicationManager			= &_world.GetSingletonComponent<ClientReplicationManager>();
		connection					= &_world.GetSingletonComponent<ClientConnectionManager>();
		linkingContext			    = &_world.GetSingletonComponent<LinkingContext>();
		rpcManager					= &_world.GetSingletonComponent<RPCManager>();
		game						= &_world.GetSingletonComponent<Game>();

		connection->onServerDisconnected.Connect( &DeliveryNotificationManager::DeleteHost, deliveryNotificationManager );
		rpcManager->onShiftFrameIndex.Connect( &ClientNetworkManager::ShiftFrameIndex, this );
		rpcManager->onSpawnShip.Connect( &ClientNetworkManager::SpawnShip, this );

		// Bind socket
		sf::Socket::Status socketStatus = sf::Socket::Disconnected;
		for( int tryIndex = 0; tryIndex < 10 && socketStatus != sf::Socket::Done; tryIndex++ )
		{
			Debug::Log() << game->name << "bind on port " << connection->clientPort << Debug::Endl();
			socketStatus = connection->socket.Bind( connection->clientPort );
			if( socketStatus != sf::Socket::Done )
			{
				Debug::Warning() << game->name << " bind failed" << Debug::Endl();
				connection->clientPort++; // try bind on the next port ( useful when using multiple clients on the same machine )
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Stop( EcsWorld& _world )
	{
		connection->socket.Unbind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Update( EcsWorld& _world )
	{
		replicationManager->ReplicateSingletons( _world );

		// spawns spaceship
		if( spaceshipSpawnFrameIndex != 0 && game->frameIndex >= spaceshipSpawnFrameIndex )
		{
			assert( spaceshipNetID != 0 );

			spaceshipSpawnFrameIndex = 0;
			spaceshipHandle = Game::SpawnSpaceship( _world );
			linkingContext->AddEntity( spaceshipHandle, spaceshipNetID );

			const EntityID spaceshipID = _world.GetEntityID( spaceshipHandle );
			if( spaceshipHandle != 0 )
			{
				_world.AddComponent<PlayerController>( spaceshipID );
			}
		}

		// streams input
		if( spaceshipHandle != 0  && synced )
		{
			const EntityID entityID = _world.GetEntityID( spaceshipHandle );
			const PlayerInput& input = _world.GetComponent<PlayerInput>( entityID );

			PacketInput packetInput;
			packetInput.frameIndex = game->frameIndex;
			packetInput.orientation = input.orientation;
			packetInput.left = input.left;
			packetInput.forward = input.forward;
			packetInput.boost = input.boost;
			packetInput.fire = input.fire;
			inputs.push( packetInput );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::ShiftFrameIndex( const int _framesDelta )
	{
		game->frameIndex += _framesDelta;
		Debug::Warning() << "Shifted client frame index : " << _framesDelta << Debug::Endl();
		synced = true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::SpawnShip( NetID _spaceshipID, FrameIndex _frameIndex )
	{
		if( spaceshipNetID == 0 )
		{
			spaceshipSpawnFrameIndex = _frameIndex;
			spaceshipNetID = _spaceshipID;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::NetworkReceive()
	{
		// receive
		Packet			packet;
		sf::IpAddress	receiveIP;
		unsigned short	receivePort;

		sf::Socket::Status socketStatus;
		do
		{
			packet.Clear();
			socketStatus = connection->socket.Receive( packet, receiveIP, receivePort );

			// only receive from the server
			if( receiveIP != connection->serverIP || receivePort != connection->serverPort )
			{
				continue;
			}

			switch( socketStatus )
			{
			case sf::UdpSocket::Done:
			{
				connection->serverLastResponse = Time::Get().ElapsedSinceStartup();

				// read the first packet type separately
				PacketType packetType = packet.ReadType();
				if( packetType == PacketType::Ack )
				{
					packet.onlyContainsAck = true;
				}

				if( !deliveryNotificationManager->ValidatePacket( packet ) )
				{
					continue;
				}

				// Process packet
				bool packetValid = true;
				while( packetValid )
				{
					switch( packetType )
					{
					case PacketType::Ack:
					{
						PacketAck packetAck;
						packetAck.Read( packet );
						deliveryNotificationManager->Receive( packetAck );
					}break;
					case PacketType::Ping:
					{
						PacketPing packetPing;
						packetPing.Read( packet );
						connection->ProcessPacket( packetPing, game->frameIndex );
					} break;
					case PacketType::LoggedIn:
					{
						PacketLoginSuccess packetLogin;
						packetLogin.Read( packet );
						connection->ProcessPacket( packetLogin );
					} break;
					case PacketType::Replication:
					{
						PacketReplication packetReplication;
						packetReplication.Read( packet );
						replicationManager->ProcessPacket( packetReplication );
					} break;
					case PacketType::PlayerGameState:
					{
						PacketPlayerGameState packetPlayerGameState;
						packetPlayerGameState.Read( packet );
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

		deliveryNotificationManager->ProcessTimedOutPackets();
		connection->DetectServerTimout();		
		replicationManager->ReplicateRPC( *rpcManager );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::NetworkSend()
	{
		// create packet
		Packet packet( deliveryNotificationManager->GetNextPacketTag() );

		// write packet
		connection->Write( packet );

		if( !inputs.empty() )
		{
			const PacketInput& lastInput = inputs.front();
			inputs.pop();
			assert( lastInput.frameIndex == game->frameIndex );
			lastInput.Write( packet );
		}

		if( packet.GetSize() == sizeof( PacketTag ) ) { packet.onlyContainsAck = true; }

		deliveryNotificationManager->Write( packet );

		// send packet, don't send empty packets
		if( packet.GetSize() > sizeof( PacketTag ) )
		{
			deliveryNotificationManager->RegisterPacket( packet );
			connection->bandwidth = 1.f / game->logicDelta * float( packet.GetSize() ) / 1000.f; // in Ko/s
			connection->socket.Send( packet, connection->serverIP, connection->serverPort );
		}
		else
		{
			deliveryNotificationManager->hostDatas[0].nextPacketTag--;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Save( const SingletonComponent& _component, Json& _json )
	{
		const ClientNetworkManager& netManager = static_cast<const ClientNetworkManager&>( _component );

	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Load( SingletonComponent& _component, const Json& _json )
	{
		ClientNetworkManager& netManager = static_cast<ClientNetworkManager&>( _component );

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