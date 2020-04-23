#include "game/fanGameServer.hpp"

#include "core/time/fanProfiler.hpp"
#include "core/time/fanTime.hpp"
#include "network/fanPacket.hpp"

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
#include "game/fanGameTags.hpp"

#include "network/singletonComponents/fanServerConnectionManager.hpp"
#include "network/singletonComponents/fanDeliveryNotificationManager.hpp"
#include "network/singletonComponents/fanServerReplicationManager.hpp"
#include "network/singletonComponents/fanRPCManager.hpp"
#include "network/singletonComponents/fanServerNetworkManager.hpp"
#include "game/singletonComponents/fanCollisionManager.hpp"
#include "game/singletonComponents/fanSolarEruption.hpp"
#include "game/singletonComponents/fanGameCamera.hpp"
#include "game/singletonComponents/fanSunLight.hpp"
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

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	GameServer::GameServer( const std::string _name )
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
		// net singleton components
		world.AddSingletonComponentType<ServerConnectionManager>();
		world.AddSingletonComponentType<DeliveryNotificationManager>();
		world.AddSingletonComponentType<ServerReplicationManager>();
		world.AddSingletonComponentType<RPCManager>();
		world.AddSingletonComponentType<ServerNetworkManager>();
		
		world.AddTagType<tag_boundsOutdated>();
		world.AddTagType<tag_sunlight_occlusion>();

		Game& game = world.GetSingletonComponent<Game>();
		game.gameServer = this;
		game.name = _name;

		// connect host creation/deletion callbacks
		ServerConnectionManager& connection = world.GetSingletonComponent<ServerConnectionManager>();
		DeliveryNotificationManager& deliveryNotificationManager = world.GetSingletonComponent<DeliveryNotificationManager>();
		connection.onClientCreated.Connect( &DeliveryNotificationManager::CreateHost, &deliveryNotificationManager );
		connection.onClientDeleted.Connect( &DeliveryNotificationManager::DeleteHost, &deliveryNotificationManager );
		ServerReplicationManager& replicationManager = world.GetSingletonComponent<ServerReplicationManager>();
		connection.onClientCreated.Connect( &ServerReplicationManager::CreateHost, &replicationManager );
		connection.onClientDeleted.Connect( &ServerReplicationManager::DeleteHost, &replicationManager );
		ServerNetworkManager& networkManager = world.GetSingletonComponent<ServerNetworkManager>();
		connection.onClientCreated.Connect( &ServerNetworkManager::CreateHost, &networkManager );
		connection.onClientDeleted.Connect( &ServerNetworkManager::DeleteHost, &networkManager );
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::Start()
	{
		Game& gameData = world.GetSingletonComponent<Game>();

		// init network
		ServerConnectionManager& connection = world.GetSingletonComponent<ServerConnectionManager>();
		Debug::Log() << gameData.name << " bind on port " << connection.serverPort << Debug::Endl();
		if( connection.socket.Bind( connection.serverPort ) != sf::Socket::Done )
		{
			Debug::Error() << gameData.name << " bind failed on port " << connection.serverPort << Debug::Endl();
		}
		state = WAITING_FOR_PLAYERS;

		// init game
		S_RegisterAllRigidbodies::Run( world, world.Match( S_RegisterAllRigidbodies::GetSignature( world ) ) );
		GameCamera::CreateGameCamera( world );
		SolarEruption::Start( world );		
	}

	//================================================================================================================================
	//================================================================================================================================
	void  GameServer::Stop()
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
		ServerConnectionManager& connection = world.GetSingletonComponent<ServerConnectionManager>();
		connection.socket.Unbind();
		connection.clients.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::Pause()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::Resume()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void  GameServer::Step( const float _delta )
	{
		Game& game = world.GetSingletonComponent<Game>();
		DeliveryNotificationManager& deliveryNotificationManager = world.GetSingletonComponent<DeliveryNotificationManager>();
		ServerConnectionManager& connection = world.GetSingletonComponent<ServerConnectionManager>();
		
		game.frameIndex++;
		{
			SCOPED_PROFILE( scene_update );

			NetworkReceive();
			deliveryNotificationManager.ProcessTimedOutPackets();
			connection.DetectClientTimout();

			// physics & transforms
			PhysicsWorld& physicsWorld = world.GetSingletonComponent<PhysicsWorld>();
			S_SynchronizeMotionStateFromTransform::Run( world, world.Match( S_SynchronizeMotionStateFromTransform::GetSignature( world ) ), _delta );
			physicsWorld.dynamicsWorld->stepSimulation( _delta, 10, Time::Get().GetPhysicsDelta() );
			S_SynchronizeTransformFromMotionState::Run( world, world.Match( S_SynchronizeTransformFromMotionState::GetSignature( world ) ), _delta );
			S_MoveFollowTransforms::Run( world, world.Match( S_MoveFollowTransforms::GetSignature( world ) ) );
			S_MoveFollowTransformsUI::Run( world, world.Match( S_MoveFollowTransformsUI::GetSignature( world ) ) );

			// update
			S_MoveSpaceships::Run( world, world.Match( S_MoveSpaceships::GetSignature( world ) ), _delta );
			S_FireWeapons::Run( world, world.Match( S_FireWeapons::GetSignature( world ) ), _delta );
			S_MovePlanets::Run( world, world.Match( S_MovePlanets::GetSignature( world ) ), _delta );
			S_GenerateLightMesh::Run( world, world.Match( S_GenerateLightMesh::GetSignature( world ) ), _delta );
			S_UpdateSolarPannels::Run( world, world.Match( S_UpdateSolarPannels::GetSignature( world ) ), _delta );
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
	void GameServer::NetworkReceive()
	{
		// receive
		Packet			packet;
		sf::IpAddress	receiveIP;
		unsigned short	receivePort;

		ServerConnectionManager& connection = world.GetSingletonComponent<ServerConnectionManager>();
		DeliveryNotificationManager& deliveryNotificationManager = world.GetSingletonComponent<DeliveryNotificationManager>();
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
				HostID clientID = connection.FindClient( receiveIP, receivePort );
				if( clientID == -1 )
				{
					clientID = connection.CreateClient( receiveIP, receivePort );
				}

				connection.clients[clientID].lastResponseTime = Time::Get().ElapsedSinceStartup();

				// read the first packet type separately
				PacketType packetType = packet.ReadType();
				if( packetType == PacketType::Ack )
				{
					packet.onlyContainsAck = true;
				}

				// packet must be approved & ack must be sent
				if( !deliveryNotificationManager.ValidatePacket( packet, clientID ) ){continue; }

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
						deliveryNotificationManager.Receive( packetAck, clientID );
					}break;
					case PacketType::Hello:
					{
						PacketHello packetHello;
						packetHello.Read( packet );
						connection.ProcessPacket( clientID, packetHello );
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

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::NetworkSend()
	{
		ServerConnectionManager& connection = world.GetSingletonComponent<ServerConnectionManager>();
		DeliveryNotificationManager& deliveryNotificationManager = world.GetSingletonComponent<DeliveryNotificationManager>();
		ServerReplicationManager& replicationManager = world.GetSingletonComponent<ServerReplicationManager>();
		ServerNetworkManager& netManager = world.GetSingletonComponent<ServerNetworkManager>();

		// generates game state packet
// 		const Game& game = world.GetSingletonComponent<Game>();
// 		const SolarEruption& solarEruption = world.GetSingletonComponent<SolarEruption>();
// 		PacketGameState	gameState;
// 		gameState.frameIndex = game.frameIndex;
// 		gameState.solarEruptionStart = solarEruption.eruptionStartFrame;

		for( int i = (int)connection.clients.size() - 1; i >= 0; i-- )
		{
			Client& client = connection.clients[i];
			if( client.state == Client::State::Null )
			{
				continue;
			}

			netManager.Update( world, client.hostId );

			// create new packet			
			Packet packet( deliveryNotificationManager.GetNextPacketTag( client.hostId ) );

			// write game data
			connection.Send( packet,  client.hostId );			
			replicationManager.Send( packet, client.hostId );

			// write ack
			if( packet.GetSize() == sizeof( PacketTag ) ) { packet.onlyContainsAck = true; }
			deliveryNotificationManager.SendAck( packet, client.hostId );

			// send packet
			if( packet.GetSize() > sizeof(PacketTag ) )// don't send empty packets
			{
				deliveryNotificationManager.RegisterPacket( packet, client.hostId );
				connection.socket.Send( packet, client.ip, client.port );
			}
			else
			{
				deliveryNotificationManager.hostDatas[i].nextPacketTag --; 
			}
		}
	}
}