#include "game/singletonComponents/fanServerNetworkManager.hpp"

#include "core/fanDebug.hpp"
#include "core/time/fanTime.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "scene/fanSceneSerializable.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanTransform.hpp"
#include "network/singletonComponents/fanRPCManager.hpp"
#include "network/singletonComponents/fanDeliveryNotificationManager.hpp"
#include "network/singletonComponents/fanServerReplicationManager.hpp"
#include "network/singletonComponents/fanServerConnectionManager.hpp"
#include "network/singletonComponents/fanLinkingContext.hpp"
#include "network/singletonComponents/fanRPCManager.hpp"


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
		netManager.deliveryNotification = nullptr;
		netManager.replication = nullptr;
		netManager.linkingContext = nullptr;
		netManager.rpcManager = nullptr;
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
		connection			= &_world.GetSingletonComponent<ServerConnectionManager>();
		deliveryNotification = &_world.GetSingletonComponent<DeliveryNotificationManager>();
		replication			= &_world.GetSingletonComponent<ServerReplicationManager>();
		linkingContext		= &_world.GetSingletonComponent<LinkingContext>();
		rpcManager			= &_world.GetSingletonComponent<RPCManager>();
		game				= &_world.GetSingletonComponent<Game>();

		// connect host creation/deletion callbacks
		connection->onClientCreated.Connect( &DeliveryNotificationManager::CreateHost, deliveryNotification );
		connection->onClientDeleted.Connect( &DeliveryNotificationManager::DeleteHost, deliveryNotification );
		connection->onClientCreated.Connect( &ServerReplicationManager::CreateHost, replication );
		connection->onClientDeleted.Connect( &ServerReplicationManager::DeleteHost, replication );
		connection->onClientCreated.Connect( &ServerNetworkManager::CreateHost, this );
		connection->onClientDeleted.Connect( &ServerNetworkManager::DeleteHost, this );

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
	//================================================================================================================================
	void ServerNetworkManager::OnSyncSuccess( HostID _hostID )
	{
		Client& client = connection->clients[_hostID];
		client.synced = true;
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

				if( client.synced == true )
				{
					if( hostData.spaceshipID == 0 )
					{
						// spawns spaceship
						hostData.spaceshipHandle = Game::SpawnSpaceship( _world );
						hostData.spaceshipID = linkingContext->nextNetID++;
						linkingContext->AddEntity( hostData.spaceshipHandle, hostData.spaceshipID );

						replication->ReplicateOnClient(
							client.hostId
							, rpcManager->RPCSSpawnShip( hostData.spaceshipID, game->frameIndex + 120 )
							, ServerReplicationManager::ResendUntilReplicated
						);
					}

					if( hostData.spaceshipID != 0 )
					{
						// get the current input for this client
						while( !hostData.inputs.empty() )
						{
							const PacketInput& packetInput = hostData.inputs.front();
							if( packetInput.frameIndex < game->frameIndex )
							{
								hostData.inputs.pop();
							}
							else
							{
								break;
							}
						}

						// moves spaceship						
						if( !hostData.inputs.empty() && hostData.inputs.front().frameIndex == game->frameIndex )
						{
							const PacketInput& packetInput = hostData.inputs.front();
							hostData.inputs.pop();
							const EntityID entityID = _world.GetEntityID( hostData.spaceshipHandle );
							PlayerInput& input = _world.GetComponent<PlayerInput>( entityID );
							input.orientation = packetInput.orientation;
							input.left = packetInput.left;
							input.forward = packetInput.forward;
							input.boost = packetInput.boost;
							input.fire = packetInput.fire;
						}
						else
						{
							Debug::Warning() << "no available input from player " << i << Debug::Endl();
						}

						// generate player state
						{
							const EntityID entityID = _world.GetEntityID( hostData.spaceshipHandle );
							const Rigidbody& rb = _world.GetComponent<Rigidbody>( entityID );
							const Transform& transform = _world.GetComponent<Transform>( entityID );
							hostData.nextPlayerState.frameIndex = game->frameIndex;
							hostData.nextPlayerState.position = transform.GetPosition();
							hostData.nextPlayerState.orientation = transform.GetRotationEuler();
							hostData.nextPlayerState.velocity = rb.GetVelocity();
							hostData.nextPlayerState.angularVelocity = rb.GetAngularVelocity();
						}
					}
				}

				// sync the client frame index with the server
				const double currentTime = Time::Get().ElapsedSinceStartup();
				if( currentTime - client.lastSync > 3.f )
				{
					int max = client.framesDelta[0];
					int min = client.framesDelta[0];
					for( int i = 1; i < client.framesDelta.size(); i++ )
					{
						max = std::max( max, client.framesDelta[i] );
						min = std::min( min, client.framesDelta[i] );
					}
					if( max - min <= 1 ) // we have consistent readings
					{
						if( std::abs( min + targetFrameDifference ) > 2 ) // only sync when we have a big enough frame index difference
						{
							RPCManager& rpcManager = _world.GetSingletonComponent<RPCManager>();
							ServerReplicationManager& replication = _world.GetSingletonComponent<ServerReplicationManager>();

							Signal<HostID>& success = *replication.ReplicateOnClient(
								client.hostId
								, rpcManager.RPCShiftClientFrame( min + targetFrameDifference )
								, ServerReplicationManager::ResendUntilReplicated
							);
							client.lastSync = currentTime;
							success.Connect( &ServerNetworkManager::OnSyncSuccess, this );

							Debug::Warning() << "Shifting client " << client.hostId << " frame index : " << min + targetFrameDifference << Debug::Endl();
						}
					}
				}
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::NetworkReceive()
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
				if( !deliveryNotification->ValidatePacket( packet, clientID ) ) { continue; }

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
						deliveryNotification->Receive( packetAck, clientID );
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
					case PacketType::PlayerInput:
					{
						PacketInput packetInput;
						packetInput.Read( packet );
						hostDatas[clientID].inputs.push( packetInput );
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

		deliveryNotification->ProcessTimedOutPackets();
		connection->DetectClientTimout();
	}

	//================================================================================================================================
	// sends data to all clients
	//================================================================================================================================
	void ServerNetworkManager::NetworkSend()
	{
		for( int i = (int)connection->clients.size() - 1; i >= 0; i-- )
		{
			Client& client = connection->clients[i];
			HostData& hostData = hostDatas[i];
			if( client.state == Client::State::Null )
			{
				continue;
			}

			// create new packet			
			Packet packet( deliveryNotification->GetNextPacketTag( client.hostId ) );

			// write game data
			if( hostData.spaceshipID != 0 )
			{
				assert( hostData.nextPlayerState.frameIndex == game->frameIndex );
				hostData.nextPlayerState.Write( packet );
			}

			connection->Write( packet, client.hostId, game->frameIndex );
			replication->Write( packet, client.hostId );

			// write ack
			if( packet.GetSize() == sizeof( PacketTag ) ) { packet.onlyContainsAck = true; }
			deliveryNotification->Write( packet, client.hostId );

			// send packet
			if( packet.GetSize() > sizeof( PacketTag ) )// don't send empty packets
			{
				deliveryNotification->RegisterPacket( packet, client.hostId );
				client.bandwidth = 1.f / game->logicDelta * float( packet.GetSize() ) / 1000.f; // in Ko/s
				connection->socket.Send( packet, client.ip, client.port );
			}
			else
			{
				deliveryNotification->hostDatas[i].nextPacketTag--;
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
			ImGui::Columns( 2 );
			ImGui::Text( "id" );			ImGui::NextColumn();
			ImGui::Text( "input buffer size");  ImGui::NextColumn();
			

			for (int i = 0; i < netManager.hostDatas.size(); i++)
			{
				HostData& data = netManager.hostDatas[i];
				if( !data.isNull )
				{
					ImGui::Text( "%d", i ); 
					ImGui::NextColumn();

					ImGui::Text( "%d", data.inputs.size() );
					ImGui::NextColumn();
				}
			}
			ImGui::Columns( 1 );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}