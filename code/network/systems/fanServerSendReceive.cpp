#include "network/systems/fanServerSendReceive.hpp"

#include "ecs/fanEcsWorld.hpp"

#include "core/time/fanTime.hpp"
#include "network/components/fanHostConnection.hpp"
#include "network/components/fanHostGameData.hpp"
#include "network/components/fanHostReplication.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "network/singletonComponents/fanHostManager.hpp"
#include "network/singletonComponents/fanServerConnection.hpp"
#include "game/singletonComponents/fanGame.hpp"

namespace fan
{	
	//================================================================================================================================
	//================================================================================================================================
	Signature S_ServerSend::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<HostConnection>() |
			_world.GetSignature<HostGameData>() |
			_world.GetSignature<HostReplication>() |
			_world.GetSignature<ReliabilityLayer>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ServerSend::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		const HostManager& hostManager = _world.GetSingletonComponent<HostManager>();
		const Game& game = _world.GetSingletonComponent<Game>();
		ServerConnection& connection = _world.GetSingletonComponent<ServerConnection>();

		for( EntityID entityID : _entities )
		{
			HostConnection& hostConnection = _world.GetComponent< HostConnection >( entityID );
			HostGameData& hostData = _world.GetComponent< HostGameData >( entityID );
			HostReplication& hostReplication = _world.GetComponent< HostReplication >( entityID );
			ReliabilityLayer& reliabilityLayer = _world.GetComponent< ReliabilityLayer >( entityID );

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

	//================================================================================================================================
	//================================================================================================================================
	void S_ServerReceive::Run( EcsWorld& _world )
	{
		HostManager& hostManager = _world.GetSingletonComponent<HostManager>();
		ServerConnection& connection = _world.GetSingletonComponent<ServerConnection>();
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

			// Don't receive from itself
			if( receivePort == connection.serverPort ) { continue; }

			switch( socketStatus )
			{
			case sf::UdpSocket::Done:
			{
				// create / get client
				EntityHandle clientHandle = hostManager.FindHost( receiveIP, receivePort );
				if( clientHandle == 0 )
				{
					clientHandle = hostManager.CreateHost( receiveIP, receivePort );
				}
				const EntityID entityID = _world.GetEntityID( clientHandle );

				HostGameData& hostData = _world.GetComponent< HostGameData >( entityID );
				ReliabilityLayer& reliabilityLayer = _world.GetComponent<ReliabilityLayer>( entityID );
				HostConnection& hostConnection = _world.GetComponent<HostConnection>( entityID );
				hostConnection.lastResponseTime = Time::Get().ElapsedSinceStartup();

				// read the first packet type separately
				PacketType packetType = packet.ReadType();
				if( packetType == PacketType::Ack )
				{
					packet.onlyContainsAck = true;
				}
				else if( packetType == PacketType::Hello )
				{
					// disconnections can cause the reliability layer tags to be off
					reliabilityLayer.expectedPacketTag = packet.tag;
				}


				// packet must be approved & ack must be sent				
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
					case PacketType::Hello:
					{
						PacketHello packetHello;
						packetHello.Read( packet );
						hostConnection.ProcessPacket( packetHello );
					} break;
					case PacketType::Disconnect:
					{
						PacketDisconnect packetDisconnect;
						packetDisconnect.Read( packet );
						hostManager.DeleteHost( clientHandle );
					} break;
					case PacketType::Ping:
					{
						PacketPing packetPing;
						packetPing.Read( packet );
						hostConnection.ProcessPacket( packetPing, game.frameIndex, game.logicDelta );
					} break;
					case PacketType::PlayerInput:
					{
						PacketInput packetInput;
						packetInput.Read( packet );
						if( hostConnection.state == HostConnection::Connected )
						{
							hostData.ProcessPacket( packetInput );
						}
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
			{
				Debug::Warning() << "socket.receive: an unexpected error happened " << Debug::Endl();
			} break;
			case sf::UdpSocket::Partial:
			case sf::UdpSocket::NotReady:
			{
				// do nothing
			} break;
			case sf::UdpSocket::Disconnected:
			{
				// disconnect client
			} break;
			default:
				assert( false );
				break;
			}
		} while( socketStatus == sf::UdpSocket::Done );
	}
}