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
#include "network/components/fanClientReplication.hpp"
#include "network/components/fanClientConnection.hpp"
#include "network/singletonComponents/fanLinkingContext.hpp"
#include "network/components/fanClientRPC.hpp"
#include "network/components/fanClientGameData.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "network/systems/fanTimeout.hpp"
#include "network/systems/fanClientUpdates.hpp"

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
		netManager.playerPersistent = nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Start( EcsWorld& _world )
	{
		// Create player persistent scene node
		Scene& scene			= _world.GetSingletonComponent<Scene>();
		playerPersistent		= & scene.CreateSceneNode( "persistent", scene.root );
		EntityID persistentID	= _world.GetEntityID( playerPersistent->handle );
		_world.AddComponent<ReliabilityLayer>( persistentID );
		_world.AddComponent<ClientConnection>( persistentID );
		_world.AddComponent<ClientReplication>( persistentID );
		_world.AddComponent<ClientRPC>( persistentID );
		_world.AddComponent<ClientGameData>( persistentID );

		// connect rpc
		Game& game = _world.GetSingletonComponent<Game>();
		ClientRPC& rpcManager = _world.GetComponent<ClientRPC>( persistentID );
		ClientGameData& gameData = _world.GetComponent<ClientGameData>( persistentID );
		rpcManager.onShiftFrameIndex.Connect( &ClientGameData::OnShiftFrameIndex, &gameData );
		rpcManager.onShiftFrameIndex.Connect( &Game::OnShiftFrameIndex, &game );
		rpcManager.onSpawnShip.Connect( &ClientGameData::OnSpawnShip, &gameData );

		// Bind socket
		ClientConnection& connection = _world.GetComponent<ClientConnection>( persistentID );
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
		const EntityID persistentID = _world.GetEntityID( playerPersistent->handle );		
		ClientConnection& connection = _world.GetComponent<ClientConnection>( persistentID );
		connection.state = ClientConnection::ClientState::Stopping;
		S_ClientNetworkSend::Run( _world, _world.Match( S_ClientNetworkSend::GetSignature( _world ) ) );// send a last packet
		connection.socket.Unbind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::NetworkReceive( EcsWorld& _world )
	{
		S_ProcessTimedOutPackets::Run( _world, _world.Match( S_ProcessTimedOutPackets::GetSignature( _world ) ) );

		const EntityID persistentID = _world.GetEntityID( playerPersistent->handle );
		ReliabilityLayer& reliabilityLayer	= _world.GetComponent<ReliabilityLayer>	( persistentID );
		ClientConnection& connection		= _world.GetComponent<ClientConnection>	( persistentID);
		ClientReplication& replication		= _world.GetComponent<ClientReplication>( persistentID );
		ClientRPC& rpc						= _world.GetComponent<ClientRPC>		( persistentID);
		ClientGameData& gameData			= _world.GetComponent<ClientGameData>( persistentID );
		Game& game							= _world.GetSingletonComponent<Game>();

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

		
		connection.DetectServerTimout();		
		replication.ReplicateRPC( rpc );
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