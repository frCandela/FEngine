#include "game/fanGameClient.hpp"

#include "core/time/fanProfiler.hpp"
#include "core/time/fanTime.hpp"

#include "scene/systems/fanSynchronizeMotionStates.hpp"
#include "scene/systems/fanRegisterPhysics.hpp"
#include "scene/systems/fanUpdateParticles.hpp"
#include "scene/systems/fanEmitParticles.hpp"
#include "scene/systems/fanGenerateParticles.hpp"
#include "scene/systems/fanUpdateBounds.hpp"
#include "scene/systems/fanUpdateTimers.hpp"
#include "scene/systems/fanUpdateTransforms.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanDirectionalLight.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanParticle.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "scene/components/fanBounds.hpp"
#include "scene/components/fanExpirationTime.hpp"
#include "scene/components/fanFollowTransform.hpp"
#include "scene/components/ui/fanFollowTransformUI.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/singletonComponents/fanRenderWorld.hpp"
#include "scene/singletonComponents/fanScenePointers.hpp"
#include "scene/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/fanSceneTags.hpp"
#include "network/fanPacket.hpp"
#include "network/singletonComponents/fanClientConnectionManager.hpp"
#include "network/singletonComponents/fanClientReplicationManager.hpp"
#include "network/singletonComponents/fanDeliveryNotificationManager.hpp"
#include "network/singletonComponents/fanRPCManager.hpp"
#include "game/fanGameTags.hpp"

#include "game/singletonComponents/fanSunLight.hpp"
#include "game/singletonComponents/fanGameCamera.hpp"
#include "game/singletonComponents/fanCollisionManager.hpp"
#include "game/singletonComponents/fanSolarEruption.hpp"
#include "game/singletonComponents/fanGame.hpp"

#include "game/systems/fanUpdatePlanets.hpp"
#include "game/systems/fanUpdateSpaceships.hpp"
#include "game/systems/fanUpdateGameCamera.hpp"
#include "game/systems/fanUpdateWeapons.hpp"
#include "game/systems/fanUpdatePlayerInput.hpp"
#include "game/systems/fanUpdateEnergy.hpp"
#include "game/systems/fanUpdateGameUI.hpp"
#include "game/systems/fanParticlesOcclusion.hpp"

#include "game/components/fanPlanet.hpp"
#include "game/components/fanSpaceShip.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/components/fanWeapon.hpp"
#include "game/components/fanBullet.hpp"
#include "game/components/fanBattery.hpp"
#include "game/components/fanSolarPanel.hpp"
#include "game/components/fanHealth.hpp"
#include "game/components/fanSpaceshipUI.hpp"
#include "game/components/fanDamage.hpp"
#include "game/components/fanPlayerController.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	GameClient::GameClient( const std::string _name )
	{
		// base components
		world.AddComponentType<SceneNode>();
		world.AddComponentType<Transform>();
		world.AddComponentType<DirectionalLight>();
		world.AddComponentType<PointLight>();
		world.AddComponentType<MeshRenderer>();
		world.AddComponentType<Material>();
		world.AddComponentType<Camera>();
		world.AddComponentType<ParticleEmitter>();
		world.AddComponentType<Particle>();
		world.AddComponentType<Rigidbody>();
		world.AddComponentType<MotionState>();
		world.AddComponentType<BoxShape>();
		world.AddComponentType<SphereShape>();
		world.AddComponentType<TransformUI>();
		world.AddComponentType<UIRenderer>();
		world.AddComponentType<Bounds>();
		world.AddComponentType<ExpirationTime>();
		world.AddComponentType<FollowTransform>();
		world.AddComponentType<ProgressBar>();
		world.AddComponentType<FollowTransformUI>();

		// game components
		world.AddComponentType<Planet>();
		world.AddComponentType<SpaceShip>();
		world.AddComponentType<PlayerInput>();
		world.AddComponentType<Weapon>();
		world.AddComponentType<Bullet>();
		world.AddComponentType<Battery>();
		world.AddComponentType<SolarPanel>();
		world.AddComponentType<Health>();
		world.AddComponentType<SpaceshipUI>();
		world.AddComponentType<Damage>();
		world.AddComponentType<PlayerController>();

		// base singleton components
		world.AddSingletonComponentType<Scene>();
		world.AddSingletonComponentType<RenderWorld>();
		world.AddSingletonComponentType<PhysicsWorld>();
		world.AddSingletonComponentType<ScenePointers>();
		// game singleton components
		world.AddSingletonComponentType<SunLight>();
		world.AddSingletonComponentType<GameCamera>();
		world.AddSingletonComponentType<CollisionManager>();
		world.AddSingletonComponentType<Game>();
		world.AddSingletonComponentType<SolarEruption>();
		// network singleton components
		world.AddSingletonComponentType<DeliveryNotificationManager>();
		world.AddSingletonComponentType<ClientConnectionManager>();
		world.AddSingletonComponentType<ClientReplicationManager>();
		world.AddSingletonComponentType<RPCManager>();

		world.AddTagType<tag_boundsOutdated>();
		world.AddTagType<tag_sunlight_occlusion>();

		Game& game = world.GetSingletonComponent<Game>();
		game.gameClient = this;
		game.name = _name;

		ClientConnectionManager& connection = world.GetSingletonComponent<ClientConnectionManager>();
		DeliveryNotificationManager& deliveryNotificationManager = world.GetSingletonComponent<DeliveryNotificationManager>();
		connection.onServerDisconnected.Connect( &DeliveryNotificationManager::DeleteHost, &deliveryNotificationManager );
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::Start()
	{
		Game& gameData = world.GetSingletonComponent<Game>();

		// Bind socket
		ClientConnectionManager& connection = world.GetSingletonComponent<ClientConnectionManager>();		
		sf::Socket::Status socketStatus = sf::Socket::Disconnected;
		for (int tryIndex = 0; tryIndex < 10 && socketStatus != sf::Socket::Done; tryIndex++)
		{
			Debug::Log() << gameData.name << "bind on port " << connection.clientPort << Debug::Endl();
			socketStatus = connection.socket.Bind( connection.clientPort );
			if( socketStatus != sf::Socket::Done )
			{
				Debug::Warning() << gameData.name << " bind failed" << Debug::Endl();
				connection.clientPort++; // try bind on the next port ( useful when using multiple clients on the same machine )
			}
		}

		// Create remote host for the server
		DeliveryNotificationManager& deliveryNotificationManager = world.GetSingletonComponent<DeliveryNotificationManager>();
		deliveryNotificationManager.CreateHost();

		// Init game
		S_RegisterAllRigidbodies::Run( world, world.Match( S_RegisterAllRigidbodies::GetSignature( world ) ) );
		GameCamera::CreateGameCamera( world );
		SolarEruption::Start( world );

// 		EntityID spaceshipID = Game::SpawnSpaceship( world );
// 		if( spaceshipID != 0 )
// 		{
// 			world.AddComponent<PlayerController>( spaceshipID );
// 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  GameClient::Stop()
	{
		// clears the physics world
		PhysicsWorld& physicsWorld = world.GetSingletonComponent<PhysicsWorld>();
		S_UnregisterAllRigidbodies::Run( world, world.Match( S_UnregisterAllRigidbodies::GetSignature( world ) ) );
		physicsWorld.rigidbodiesHandles.clear();

		// clears the particles mesh
		RenderWorld& renderWorld = world.GetSingletonComponent<RenderWorld>();
		renderWorld.particlesMesh.LoadFromVertices( {} );

		GameCamera::DeleteGameCamera( world );

		// clears the network
		ClientConnectionManager& connection = world.GetSingletonComponent<ClientConnectionManager>();
		connection.socket.Unbind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::Pause()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::Resume()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void  GameClient::Step( const float _delta )
	{
		Game& game = world.GetSingletonComponent<Game>();
		DeliveryNotificationManager& deliveryNotificationManager = world.GetSingletonComponent<DeliveryNotificationManager>();
		ClientConnectionManager& connection = world.GetSingletonComponent<ClientConnectionManager>();
		ClientReplicationManager& replicationManager = world.GetSingletonComponent<ClientReplicationManager>();
		RPCManager& rpcManager = world.GetSingletonComponent<RPCManager>();
		game.frameIndex++;
		{
			SCOPED_PROFILE( scene_update );

			NetworkReceive();
			deliveryNotificationManager.ProcessTimedOutPackets();
			connection.DetectServerTimout();
			replicationManager.ReplicateSingletons( world );
			replicationManager.ReplicateRPC( rpcManager );

			// physics & transforms
			PhysicsWorld& physicsWorld = world.GetSingletonComponent<PhysicsWorld>();
			S_SynchronizeMotionStateFromTransform::Run( world, world.Match( S_SynchronizeMotionStateFromTransform::GetSignature( world ) ), _delta );
			physicsWorld.dynamicsWorld->stepSimulation( _delta, 10, Time::Get().GetPhysicsDelta() );
			S_SynchronizeTransformFromMotionState::Run( world, world.Match( S_SynchronizeTransformFromMotionState::GetSignature( world ) ), _delta );
			S_MoveFollowTransforms::Run( world, world.Match( S_MoveFollowTransforms::GetSignature( world ) ) );
			S_MoveFollowTransformsUI::Run( world, world.Match( S_MoveFollowTransformsUI::GetSignature( world ) ) );

			// update
			S_RefreshPlayerInput::Run( world, world.Match( S_RefreshPlayerInput::GetSignature( world ) ), _delta );
			S_MoveSpaceships::Run( world, world.Match( S_MoveSpaceships::GetSignature( world ) ), _delta );
			S_FireWeapons::Run( world, world.Match( S_FireWeapons::GetSignature( world ) ), _delta );
			S_MovePlanets::Run( world, world.Match( S_MovePlanets::GetSignature( world ) ), _delta );
			S_GenerateLightMesh::Run( world, world.Match( S_GenerateLightMesh::GetSignature( world ) ), _delta );
			S_UpdateSolarPannels::Run(world, world.Match( S_UpdateSolarPannels::GetSignature( world ) ), _delta );
			S_RechargeBatteries::Run( world, world.Match( S_RechargeBatteries::GetSignature( world ) ), _delta );
			S_UpdateExpirationTimes::Run( world, world.Match( S_UpdateExpirationTimes::GetSignature( world ) ), _delta );
			S_EruptionDamage::Run( world, world.Match( S_EruptionDamage::GetSignature( world ) ), _delta );
			S_UpdateGameUiValues::Run( world, world.Match( S_UpdateGameUiValues::GetSignature( world ) ), _delta );
			S_UpdateGameUiPosition::Run( world, world.Match( S_UpdateGameUiPosition::GetSignature( world ) ), _delta );

			SolarEruption::Step( world, _delta );

			S_PlayerDeath::Run( world, world.Match( S_PlayerDeath::GetSignature( world ) ), _delta );

			// late update
			S_ParticlesOcclusion::Run( world, world.Match( S_ParticlesOcclusion::GetSignature( world ) ), _delta );
			S_UpdateParticles::Run( world, world.Match( S_UpdateParticles::GetSignature( world ) ), _delta );
			S_EmitParticles::Run( world, world.Match( S_EmitParticles::GetSignature( world ) ), _delta );
			S_GenerateParticles::Run( world, world.Match( S_GenerateParticles::GetSignature( world ) ), _delta );
			S_UpdateBoundsFromRigidbody::Run( world, world.Match( S_UpdateBoundsFromRigidbody::GetSignature( world ) ), _delta );
			S_UpdateBoundsFromModel::Run( world, world.Match( S_UpdateBoundsFromModel::GetSignature( world ) ), _delta );
			S_UpdateBoundsFromTransform::Run( world, world.Match( S_UpdateBoundsFromTransform::GetSignature( world ) ), _delta );

			S_UpdateGameCamera::Run( world, world.Match( S_UpdateGameCamera::GetSignature( world ) ), _delta );

			NetworkSend();
		}

		{
			// end frame
			SCOPED_PROFILE( scene_endFrame );
			world.SortEntities();
			world.RemoveDeadEntities();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::NetworkReceive()
	{
		// receive
		Packet			packet;
		sf::IpAddress	receiveIP;
		unsigned short	receivePort;

		DeliveryNotificationManager& deliveryNotificationManager = world.GetSingletonComponent<DeliveryNotificationManager>();
		ClientConnectionManager& connection = world.GetSingletonComponent<ClientConnectionManager>();
		ClientReplicationManager& replicationManager = world.GetSingletonComponent<ClientReplicationManager>();

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

				if( !deliveryNotificationManager.ValidatePacket( packet ) ) 
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
						deliveryNotificationManager.Receive( packetAck );
					}break;
					case PacketType::Ping:
					{
						PacketPing packetPing;
						packetPing.Read( packet );
						connection.ProcessPacket( packetPing );
					} break;
					case PacketType::LoggedIn:
					{
						PacketLoginSuccess packetLogin;
						packetLogin.Read( packet );
						connection.ProcessPacket( packetLogin );
					} break;
					case PacketType::Replication:
					{
						PacketReplication packetReplication;
						packetReplication.Read( packet );
						replicationManager.ProcessPacket( packetReplication );
					} break;

// 					case PacketType::GameState:
// 					{
// 						PacketGameState packetGameState;
// 						packetGameState.Read( packet );
// 						replicationManager.ProcessPacket( packetGameState );
// 					} break;



					
// 					case PacketType::START:
// 					{
// 						randomFlags |= MUST_ACK_START;
// 						if( state == CONNECTED )
// 						{
// 							PacketStart packetStart;
// 							packetStart.Load( packet );
// 							game.frameStart = packetStart.frameStartIndex;
// 							state = STARTING;
// 							Debug::Highlight() << "game started" << Debug::Endl();
// 						}
// 					} break;
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
					else { 
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
		}
		while( socketStatus == sf::UdpSocket::Done );
	}

	void  GameClient::OnTestFailure( HostID _client )
	{
		Debug::Log( "failure" );
	}
	void  GameClient::OnTestSuccess( HostID _client )
	{
		Debug::Log( "success" );
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::NetworkSend()
	{
		ClientConnectionManager& connection = world.GetSingletonComponent<ClientConnectionManager>();
		DeliveryNotificationManager& deliveryNotificationManager = world.GetSingletonComponent<DeliveryNotificationManager>();

		// create packet
		Packet packet( deliveryNotificationManager.GetNextPacketTag() );

		// write packet
		connection.Send( packet );

		if( packet.GetSize() == sizeof( PacketTag ) ) { packet.onlyContainsAck = true; }

		deliveryNotificationManager.SendAck( packet );

		// send packet, don't send empty packets
		if( packet.GetSize() > sizeof( PacketTag ) )
		{
			deliveryNotificationManager.RegisterPacket( packet );
			connection.socket.Send( packet, connection.serverIP, connection.serverPort );
		}
		else
		{
			deliveryNotificationManager.hostDatas[0].nextPacketTag--;
		}

// 		double currentTime = Time::Get().ElapsedSinceStartup();
// 
//  		Packet packet = socket.CreatePacket(); // One packet to rule them all
// 
// 		switch( state )
// 		{
// 		case State::DISCONNECTED:
// 		{
// 			PacketLogin packetLogin;
// 			packetLogin.name = world.GetSingletonComponent<Game>().name;
// 			packetLogin.Save( packet );
// 		} break;
// 		case State::CONNECTED:
// 		{
// 			
// 		} break;
// 		case State::STARTING:
// 		{
// 
// 		} break;
// 
// 		default:
// 			assert( false );
// 			break;
// 		}
// 
// 		// server timeout 
// 		if( state == CONNECTED &&  currentTime - serverLastResponse > timeoutDuration )
// 		{
// 			Debug::Log() << "server timeout" << Debug::Endl();
// 			Debug::Highlight() << "disconnected !" << Debug::Endl();
// 			state = DISCONNECTED;
// 		}
// 
// 		// ping
// 		if( randomFlags && MUST_ACK_START )
// 		{
// 			PacketACK packetAck;
// 			packetAck.ackType = PacketType::START;
// 			packetAck.Save( packet );
// 			randomFlags &= ! MUST_ACK_START;
// 		}
// 		if( mustPingServer > 0.f )
// 		{
// 			PacketPing packetPing;
// 			packetPing.time = mustPingServer;
// 			packetPing.Save( packet );
// 			mustPingServer = -1.f;
// 		}
// 
// 		// send packet
// 		if( packet.getDataSize() > 0 )
// 		{
// 			socket.Send( packet, serverIP, serverPort );
// 		}
 	}
}