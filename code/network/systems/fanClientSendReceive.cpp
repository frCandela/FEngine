#include "network/systems/fanClientSendReceive.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanTime.hpp"
#include "network/components/fanClientConnection.hpp"
#include "network/components/fanClientGameData.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "network/components/fanClientReplication.hpp"
#include "game/singletonComponents/fanGame.hpp"

namespace fan
{	
	//================================================================================================================================
	//================================================================================================================================
	Signature S_ClientSend::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<ReliabilityLayer>() |
			_world.GetSignature<ClientConnection>() |
			_world.GetSignature<ClientGameData>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ClientSend::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

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

	//================================================================================================================================
	//================================================================================================================================
	Signature S_ClientReceive::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<ReliabilityLayer>() |
			_world.GetSignature<ClientConnection>() |
			_world.GetSignature<ClientGameData>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ClientReceive::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		const Game& game = _world.GetSingletonComponent<Game>();

		for( EntityID entityID : _entities )
		{
			ReliabilityLayer&	reliabilityLayer = _world.GetComponent<ReliabilityLayer>( entityID );
			ClientConnection&	connection = _world.GetComponent<ClientConnection>( entityID );
			ClientReplication&	replication = _world.GetComponent<ClientReplication>( entityID );
			ClientGameData&		gameData = _world.GetComponent<ClientGameData>( entityID );

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
							gameData.ProcessPacket( packetPlayerGameState );
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
		}
	}
}