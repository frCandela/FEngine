#include "game/singletonComponents/fanServerNetworkManager.hpp"

#include "core/fanDebug.hpp"
#include "core/time/fanTime.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "scene/fanSceneSerializable.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "network/components/fanHostGameData.hpp"
#include "network/components/fanHostConnection.hpp"
#include "network/components/fanHostReplication.hpp"
#include "network/components/fanHostDeliveryNotification.hpp"
#include "network/singletonComponents/fanRPCManager.hpp"
#include "network/singletonComponents/fanServerConnectionManager.hpp"
#include "network/singletonComponents/fanLinkingContext.hpp"
#include "network/singletonComponents/fanRPCManager.hpp"
#include "network/singletonComponents/fanHostManager.hpp"
#include "network/systems/fanUpdateDeliveryNotification.hpp"

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
		netManager.connection = nullptr;
		netManager.linkingContext = nullptr;
		netManager.rpcManager = nullptr;
		netManager.game = nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::Start( EcsWorld& _world )
	{
		connection			= &_world.GetSingletonComponent<ServerConnectionManager>();
		linkingContext		= &_world.GetSingletonComponent<LinkingContext>();
		rpcManager			= &_world.GetSingletonComponent<RPCManager>();
		game				= &_world.GetSingletonComponent<Game>();

		// create the network scene root for ordering net objects
		HostManager& hostManager = _world.GetSingletonComponent<HostManager>();
		Scene& scene = _world.GetSingletonComponent<Scene>();
		hostManager.netRoot = &scene.CreateSceneNode( "net root", scene.root );

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
	}

	//================================================================================================================================
	// Updates network objects in preparation for sending it to all clients
	// @todo this should be a system
	//================================================================================================================================
	void ServerNetworkManager::Update( EcsWorld& _world )
	{
		HostManager & hostManager = _world.GetSingletonComponent<HostManager>();
		ServerConnectionManager& connection = _world.GetSingletonComponent<ServerConnectionManager>();
		for ( std::pair<HostID, EntityHandle> pair : hostManager.hostHandles )
		{			
			const HostID   hostID   = pair.first;
			const EntityID entityID = _world.GetEntityID( pair.second );
			HostConnection& hostConnection = _world.GetComponent<HostConnection>( entityID );

			if( hostConnection.state == HostConnection::Connected )
			{
				HostGameData& hostData = _world.GetComponent< HostGameData >( entityID );
				HostReplication& hostReplication = _world.GetComponent< HostReplication >( entityID );

				if( hostConnection.synced == true )
				{
					if( hostData.spaceshipID == 0 )
					{
						// spawns spaceship
						hostData.spaceshipHandle = Game::SpawnSpaceship( _world );
						hostData.spaceshipID = linkingContext->nextNetID++;
						linkingContext->AddEntity( hostData.spaceshipHandle, hostData.spaceshipID );

						hostReplication.ReplicateOnClient(
							hostConnection.hostId
							, rpcManager->RPCSSpawnShip( hostData.spaceshipID, game->frameIndex + 120 )
							, HostReplication::ResendUntilReplicated
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
							Debug::Warning() << "no available input from player " << hostID << Debug::Endl();
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
				if( currentTime - hostConnection.lastSync > 3.f )
				{
					int max = hostConnection.framesDelta[0];
					int min = hostConnection.framesDelta[0];
					for( int i = 1; i < hostConnection.framesDelta.size(); i++ )
					{
						max = std::max( max, hostConnection.framesDelta[i] );
						min = std::min( min, hostConnection.framesDelta[i] );
					}

					if( max - min <= 1 ) // we have consistent readings
					{
						if( std::abs( min + hostManager.targetFrameDifference ) > 2 ) // only sync when we have a big enough frame index difference
						{
							RPCManager& rpcManager = _world.GetSingletonComponent<RPCManager>();

							Signal<>& success = hostReplication.ReplicateOnClient(
								hostConnection.hostId
								, rpcManager.RPCShiftClientFrame( min + hostManager.targetFrameDifference )
								, HostReplication::ResendUntilReplicated
							);
							hostConnection.lastSync = currentTime;
							success.Connect( &HostConnection::OnSyncSuccess, &hostConnection );

							Debug::Warning() << "Shifting client " << hostConnection.hostId << " frame index : " << min + hostManager.targetFrameDifference << Debug::Endl();
						}
					}
				}
			}
		}
	}

	//================================================================================================================================
	// this should be a system
	//================================================================================================================================
	void ServerNetworkManager::NetworkReceive( EcsWorld& _world )
	{
		HostManager& hostManager = _world.GetSingletonComponent<HostManager>();

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
				HostID clientID = hostManager.FindHost( receiveIP, receivePort );
				if( clientID == -1 )
				{
					clientID = hostManager.CreateHost( receiveIP, receivePort );
				}
				const EntityHandle handle = hostManager.hostHandles[clientID];
				const EntityID entityID = _world.GetEntityID( handle );
				
				HostConnection& hostConnection = _world.GetComponent<HostConnection>( entityID );
				hostConnection.lastResponseTime = Time::Get().ElapsedSinceStartup();

				// read the first packet type separately
				PacketType packetType = packet.ReadType();
				if( packetType == PacketType::Ack )
				{
					packet.onlyContainsAck = true;
				}

				// packet must be approved & ack must be sent
				HostDeliveryNotification& hostDeliveryNotification = _world.GetComponent<HostDeliveryNotification>( entityID );
				if( !hostDeliveryNotification.ValidatePacket( packet, clientID ) ) { continue; }

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
						hostDeliveryNotification.ProcessPacket( packetAck, clientID );
					}break;
					case PacketType::Hello:
					{
						PacketHello packetHello;
						packetHello.Read( packet );
						hostConnection.ProcessPacket( clientID, packetHello );
					} break;
					case PacketType::Ping:
					{
						PacketPing packetPing;
						packetPing.Read( packet );
						hostConnection.ProcessPacket( clientID, packetPing, game->frameIndex, game->logicDelta );
					} break;
					case PacketType::PlayerInput:
					{
						PacketInput packetInput;
						packetInput.Read( packet );
						HostGameData& hostData = _world.GetComponent< HostGameData >( entityID );
						hostData.inputs.push( packetInput );
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

		S_ProcessTimedOutPackets::Run( _world, _world.Match( S_ProcessTimedOutPackets::GetSignature( _world ) ) );
		connection->DetectClientTimout( _world );
	}

	//================================================================================================================================
	// this should be a system
	// sends data to all clients
	//================================================================================================================================
	void ServerNetworkManager::NetworkSend( EcsWorld& _world )
	{
		HostManager& hostManager = _world.GetSingletonComponent<HostManager>();

		for( std::pair<HostID, EntityHandle> pair : hostManager.hostHandles )
		{
			const HostID   hostID = pair.first;
			const EntityID entityID = _world.GetEntityID( pair.second );
			HostConnection& hostConnection = _world.GetComponent<HostConnection>( entityID );
			HostGameData&	hostData	   = _world.GetComponent< HostGameData >( entityID );
			HostReplication& hostReplication = _world.GetComponent< HostReplication >( entityID );
			HostDeliveryNotification& deliveryNotification = _world.GetComponent< HostDeliveryNotification >( entityID );

			// create new packet			
			Packet packet( deliveryNotification.GetNextPacketTag( hostID ) );

			// write game data
			if( hostData.spaceshipID != 0 )
			{
				assert( hostData.nextPlayerState.frameIndex == game->frameIndex );
				hostData.nextPlayerState.Write( packet );
			}

			connection->Write( _world, packet, hostID );
			hostReplication.Write( packet, hostID );

			// write ack
			if( packet.GetSize() == sizeof( PacketTag ) ) { packet.onlyContainsAck = true; }
			deliveryNotification.Write( packet, hostID );

			// send packet
			if( packet.GetSize() > sizeof( PacketTag ) )// don't send empty packets
			{
				deliveryNotification.RegisterPacket( packet, hostID );
				hostConnection.bandwidth = 1.f / game->logicDelta * float( packet.GetSize() ) / 1000.f; // in Ko/s
				connection->socket.Send( packet, hostConnection.ip, hostConnection.port );
			}
			else
			{
				deliveryNotification.nextPacketTag--;
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
			ImGui::Columns( 1 );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}