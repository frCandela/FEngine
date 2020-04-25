#include "game/singletonComponents/fanServerNetworkManager.hpp"

#include "scene/fanSceneSerializable.hpp"
#include "core/time/fanTime.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "network/singletonComponents/fanRPCManager.hpp"
#include "network/singletonComponents/fanDeliveryNotificationManager.hpp"
#include "network/singletonComponents/fanServerReplicationManager.hpp"
#include "network/singletonComponents/fanServerConnectionManager.hpp"
#include "network/singletonComponents/fanLinkingContext.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( ServerNetworkManager );

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.init = &ServerNetworkManager::Init;
		_info.onGui = &ServerNetworkManager::OnGui;
		_info.save = &ServerNetworkManager::Save;
		_info.load = &ServerNetworkManager::Load;
		_info.name = "server network manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		ServerNetworkManager& netManager = static_cast<ServerNetworkManager&>( _component );
		netManager.hostDatas.clear();
		netManager.connection = nullptr;
		netManager.deliveryNotificationManager = nullptr;
		netManager.replicationManager = nullptr;
		netManager.linkingContext = nullptr;
		netManager.game = nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::CreateHost( const HostID _hostID )
	{
		if( _hostID >= hostDatas.size() )
		{
			hostDatas.resize( _hostID + 1 );
		}

		HostData& hostData = hostDatas[_hostID];
		hostData = {};
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::DeleteHost( const HostID _hostID )
	{
		hostDatas[_hostID].isNull = true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::Start( EcsWorld& _world )
	{
		connection					= &_world.GetSingletonComponent<ServerConnectionManager>();
		deliveryNotificationManager = &_world.GetSingletonComponent<DeliveryNotificationManager>();
		replicationManager			= &_world.GetSingletonComponent<ServerReplicationManager>();
		linkingContext				= &_world.GetSingletonComponent<LinkingContext>();
		game						= &_world.GetSingletonComponent<Game>();


		Debug::Log() << game->name << " bind on port " << connection->serverPort << Debug::Endl();
		if( connection->socket.Bind( connection->serverPort ) != sf::Socket::Done )
		{
			Debug::Error() << game->name << " bind failed on port " << connection->serverPort << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::Stop( EcsWorld& _world )
	{

		connection->socket.Unbind();
		connection->clients.clear();
	}

	//================================================================================================================================
	// Updates network objects in preparation for sending it to all clients
	//================================================================================================================================
	void ServerNetworkManager::Update( EcsWorld& _world )
	{
		ServerConnectionManager& connection = _world.GetSingletonComponent<ServerConnectionManager>();
		for( int i = (int)connection.clients.size() - 1; i >= 0; i-- )
		{
			Client& client = connection.clients[i];
			if( client.state == Client::State::Connected )
			{
				HostData& hostData = hostDatas[i];
				if( hostData.spaceshipID == 0 )
				{

			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::NetworkReceive( EcsWorld& _world )
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

			// Don't receive from itself
			if( receivePort == connection->serverPort ) { continue; }

			switch( socketStatus )
			{
			case sf::UdpSocket::Done:
			{
				// create / get client
				HostID clientID = connection->FindClient( receiveIP, receivePort );
				if( clientID == -1 )
				{
					clientID = connection->CreateClient( receiveIP, receivePort );
				}

				connection->clients[clientID].lastResponseTime = Time::Get().ElapsedSinceStartup();

				// read the first packet type separately
				PacketType packetType = packet.ReadType();
				if( packetType == PacketType::Ack )
				{
					packet.onlyContainsAck = true;
				}

				// packet must be approved & ack must be sent
				if( !deliveryNotificationManager->ValidatePacket( packet, clientID ) ) { continue; }

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
						deliveryNotificationManager->Receive( packetAck, clientID );
					}break;
					case PacketType::Hello:
					{
						PacketHello packetHello;
						packetHello.Read( packet );
						connection->ProcessPacket( clientID, packetHello );
					} break;
					case PacketType::Ping:
					{
						PacketPing packetPing;
						packetPing.Read( packet );
						connection->ProcessPacket( clientID, packetPing, game->frameIndex, game->logicDelta );
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

		deliveryNotificationManager->ProcessTimedOutPackets();
		connection->DetectClientTimout();
	}

	//================================================================================================================================
	// sends data to all clients
	//================================================================================================================================
	void ServerNetworkManager::NetworkSend( EcsWorld& _world )
	{
		for( int i = (int)connection->clients.size() - 1; i >= 0; i-- )
		{
			Client& client = connection->clients[i];
			if( client.state == Client::State::Null )
			{
				continue;
			}

			// create new packet			
			Packet packet( deliveryNotificationManager->GetNextPacketTag( client.hostId ) );

			// write game data
			connection->Send( packet, client.hostId, _world );
			replicationManager->Send( packet, client.hostId );

			// write ack
			if( packet.GetSize() == sizeof( PacketTag ) ) { packet.onlyContainsAck = true; }
			deliveryNotificationManager->SendAck( packet, client.hostId );

			// send packet
			if( packet.GetSize() > sizeof( PacketTag ) )// don't send empty packets
			{
				deliveryNotificationManager->RegisterPacket( packet, client.hostId );
				connection->socket.Send( packet, client.ip, client.port );
			}
			else
			{
				deliveryNotificationManager->hostDatas[i].nextPacketTag--;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::Save( const SingletonComponent& _component, Json& _json )
	{
		const ServerNetworkManager& netManager = static_cast<const ServerNetworkManager&>( _component );

	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::Load( SingletonComponent& _component, const Json& _json )
	{
		ServerNetworkManager& netManager = static_cast<ServerNetworkManager&>( _component );

	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		ServerNetworkManager& netManager = static_cast<ServerNetworkManager&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			for (int i = 0; i < netManager.hostDatas.size(); i++)
			{
				HostData& data = netManager.hostDatas[i];
				if( !data.isNull )
				{
					ImGui::Text( "client %d", i );
					ImGui::Spacing(); ImGui::Spacing();
				}
			}
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}