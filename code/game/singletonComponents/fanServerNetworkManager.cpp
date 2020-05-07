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
#include "network/components/fanReliabilityLayer.hpp"
#include "network/singletonComponents/fanRPCManager.hpp"
#include "network/singletonComponents/fanServerConnection.hpp"
#include "network/singletonComponents/fanLinkingContext.hpp"
#include "network/singletonComponents/fanRPCManager.hpp"
#include "network/singletonComponents/fanHostManager.hpp"
#include "network/systems/fanTimeout.hpp"

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
		_info.name = "server network manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		ServerNetworkManager& netManager = static_cast<ServerNetworkManager&>( _component );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::Start( EcsWorld& _world )
	{
		// create the network scene root for ordering net objects
		HostManager& hostManager = _world.GetSingletonComponent<HostManager>();
		Scene& scene = _world.GetSingletonComponent<Scene>();
		hostManager.netRoot = &scene.CreateSceneNode( "net root", scene.root );

		// bind
		ServerConnection& connection = _world.GetSingletonComponent<ServerConnection>();
		Debug::Log() << " bind on port " << connection.serverPort << Debug::Endl();
		if( connection.socket.Bind( connection.serverPort ) != sf::Socket::Done )
		{
			Debug::Error() << " bind failed on port " << connection.serverPort << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::Stop( EcsWorld& _world )
	{
		ServerConnection& connection = _world.GetSingletonComponent<ServerConnection>();
		connection.socket.Unbind();
	}

	//================================================================================================================================
	// this should be a system
	//================================================================================================================================
	void ServerNetworkManager::NetworkReceive( EcsWorld& _world )
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
				EntityHandle handle = hostManager.FindHost( receiveIP, receivePort );
				if( handle == 0 )
				{
					handle = hostManager.CreateHost( receiveIP, receivePort );
				}
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
				ReliabilityLayer& reliabilityLayer = _world.GetComponent<ReliabilityLayer>( entityID );
				if( !reliabilityLayer.ValidatePacket( packet ) ) { continue; }

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
		S_DetectClientTimout::Run( _world, _world.Match( S_DetectClientTimout::GetSignature( _world ) ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		ServerNetworkManager& netManager = static_cast<ServerNetworkManager&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Text( "Stop looking at me plz" );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}