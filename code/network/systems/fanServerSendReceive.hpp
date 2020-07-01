#include "ecs/fanEcsSystem.hpp"
#include "network/components/fanHostConnection.hpp"
#include "network/components/fanHostGameData.hpp"
#include "network/components/fanHostReplication.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "game/singletons/fanGame.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// Sends packets to all hosts
	//==============================================================================================================================================================
	struct S_ServerSend : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<HostConnection>() |
				_world.GetSignature<HostGameData>() |
				_world.GetSignature<HostReplication>() |
				_world.GetSignature<ReliabilityLayer>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			const Time& time = _world.GetSingleton<Time>();
			ServerConnection& connection = _world.GetSingleton<ServerConnection>();

			auto hostConnectionIt = _view.begin<HostConnection>();
			auto hostDataIt = _view.begin<HostGameData>();
			auto hostReplicationIt = _view.begin<HostReplication>();
			auto reliabilityLayerIt = _view.begin<ReliabilityLayer>();
			for( ; hostConnectionIt != _view.end<HostConnection>(); ++hostConnectionIt, ++hostDataIt, ++hostReplicationIt, ++reliabilityLayerIt )
			{
				HostConnection& hostConnection = *hostConnectionIt;
				const HostGameData& hostData = *hostDataIt;
				HostReplication& hostReplication = *hostReplicationIt;
				ReliabilityLayer& reliabilityLayer = *reliabilityLayerIt;

				// create new packet			
				Packet packet( reliabilityLayer.GetNextPacketTag() );

				// write game data
				if( hostData.spaceshipID != 0 )
				{
					if( hostData.nextPlayerState.frameIndex == time.frameIndex )
					{
						hostData.nextPlayerState.Write( packet );
					}
				}

				const EcsEntity entity = hostDataIt.GetEntity();
				hostConnection.Write( _world, entity, packet );
				hostReplication.Write( _world, entity, packet );

				// write ack
				if( packet.GetSize() == sizeof( PacketTag ) ) { packet.onlyContainsAck = true; }
				reliabilityLayer.Write( packet );

				// send packet
				if( packet.GetSize() > sizeof( PacketTag ) )// don't send empty packets
				{
					reliabilityLayer.RegisterPacket( packet );
					hostConnection.bandwidth = 1.f / time.logicDelta * float( packet.GetSize() ) / 1000.f; // in Ko/s
					connection.socket.Send( packet, hostConnection.ip, hostConnection.port );
				}
				else
				{
					reliabilityLayer.nextPacketTag--;
				}
			}
		}
	};

	//==============================================================================================================================================================
	// Receives packets from all hosts
	//==============================================================================================================================================================
	struct S_ServerReceive : EcsSystem
	{
		static void Run( EcsWorld& _world, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			HostManager& hostManager = _world.GetSingleton<HostManager>();
			ServerConnection& connection = _world.GetSingleton<ServerConnection>();
			Time& time = _world.GetSingleton<Time>();

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
					EcsHandle clientHandle = hostManager.FindHost( receiveIP, receivePort );
					if( clientHandle == 0 )
					{
						clientHandle = hostManager.CreateHost( _world, receiveIP, receivePort );
					}
					const EcsEntity entity = _world.GetEntity( clientHandle );

					HostGameData& hostData = _world.GetComponent< HostGameData >( entity );
					ReliabilityLayer& reliabilityLayer = _world.GetComponent<ReliabilityLayer>( entity );
					HostConnection& hostConnection = _world.GetComponent<HostConnection>( entity );
					hostConnection.lastResponseTime = Time::ElapsedSinceStartup();

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
							hostManager.DeleteHost( _world, clientHandle );
							const HostGameData& hostGameData = _world.GetComponent< HostGameData>( _world.GetEntity( clientHandle ) );
							if( hostGameData.spaceshipID != 0 )
							{
								_world.Run<S_ReplicateOnAllHosts>( ClientRPC::RPCDespawn( hostGameData.spaceshipID ), HostReplication::ResendUntilReplicated, clientHandle );
							}
						} break;
						case PacketType::Ping:
						{
							PacketPing packetPing;
							packetPing.Read( packet );
							hostConnection.ProcessPacket( packetPing, time.frameIndex, time.logicDelta );
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
	};
}