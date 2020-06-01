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
	EcsSignature S_ClientSend::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<ReliabilityLayer>() |
			_world.GetSignature<ClientConnection>() |
			_world.GetSignature<ClientGameData>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ClientSend::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		Game& game = _world.GetSingleton<Game>();

		auto reliabilityLayerIt = _view.begin<ReliabilityLayer>();
		auto connectionIt = _view.begin<ClientConnection>();
		auto gameDataIt = _view.begin<ClientGameData>();
		for( ; reliabilityLayerIt != _view.end<ReliabilityLayer>(); ++reliabilityLayerIt, ++connectionIt, ++gameDataIt )
		{
			ReliabilityLayer& reliabilityLayer = *reliabilityLayerIt;
			ClientConnection& connection = *connectionIt;
			ClientGameData& gameData = *gameDataIt;

			// create packet
			Packet packet( reliabilityLayer.GetNextPacketTag() );
			
			// write packet
			const EcsEntity entity = gameDataIt.Entity();
			connection.Write( _world, entity, packet );
			gameData.Write( _world, entity, packet );

			if( packet.GetSize() == sizeof( PacketTag ) ) { packet.onlyContainsAck = true; }

			reliabilityLayer.Write( packet );

			// send packet, don't send empty packets
			if( packet.GetSize() > sizeof( PacketTag ) )
			{
				reliabilityLayer.RegisterPacket( packet );
				connection.bandwidth = 1.f / game.logicDelta * float( packet.GetSize() ) / 1000.f; // in Ko/s
				connection.socket->Send( packet, connection.serverIP, connection.serverPort );
			}
			else
			{
				reliabilityLayer.nextPacketTag--;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_ClientReceive::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<ReliabilityLayer>() |
			_world.GetSignature<ClientConnection>() |
			_world.GetSignature<ClientReplication>() |
			_world.GetSignature<ClientGameData>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ClientReceive::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		const Game& game = _world.GetSingleton<Game>();

		auto reliabilityLayerIt = _view.begin<ReliabilityLayer>();
		auto connectionIt = _view.begin<ClientConnection>();
		auto replicationIt = _view.begin<ClientReplication>();
		auto gameDataIt = _view.begin<ClientGameData>();
		for( ; reliabilityLayerIt != _view.end<ReliabilityLayer>(); ++reliabilityLayerIt, ++connectionIt, ++replicationIt, ++gameDataIt )
		{
			ReliabilityLayer& reliabilityLayer = *reliabilityLayerIt;
			ClientConnection& connection = *connectionIt;
			ClientReplication& replication = *replicationIt;
			ClientGameData& gameData = *gameDataIt;

			// receive
			Packet			packet;
			sf::IpAddress	receiveIP;
			unsigned short	receivePort;

			sf::Socket::Status socketStatus;
			do
			{
				packet.Clear();
				socketStatus = connection.socket->Receive( packet, receiveIP, receivePort );

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