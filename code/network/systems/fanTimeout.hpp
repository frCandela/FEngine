#include "ecs/fanEcsSystem.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "network/singletons/fanHostManager.hpp"
#include "network/components/fanHostConnection.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "network/components/fanHostGameData.hpp"
#include "network/components/fanClientRPC.hpp"
#include "network/systems/fanHostReplication.hpp"

namespace fan
{
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct S_DetectHostTimout : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<HostConnection>() 
				| _world.GetSignature<SceneNode>() 
				| _world.GetSignature<HostGameData>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			HostManager& hostManager = _world.GetSingleton<HostManager>();
			const double currentTime = Time::ElapsedSinceStartup();

			auto sceneNodeIt = _view.begin<SceneNode>();
			auto hostConnectionit = _view.begin<HostConnection>();
			auto hostGameDataIt = _view.begin<HostGameData>();
			for( ; hostConnectionit != _view.end<HostConnection>(); ++hostConnectionit, ++sceneNodeIt, ++hostGameDataIt )
			{
				const HostConnection& connection = *hostConnectionit;
				const SceneNode& sceneNode = *sceneNodeIt;

				// packets are sorted, so all timed out packets must be at front
				if( connection.state == HostConnection::Connected )
				{
					if( connection.lastResponseTime + connection.timeoutDelay < currentTime )
					{
						Debug::Log() << "client timeout " << Debug::Endl();

						hostManager.DeleteHost( _world, sceneNode.handle );
						const HostGameData& hostGameData = *hostGameDataIt;
						if( hostGameData.spaceshipID != 0 )
						{
							_world.Run<S_ReplicateOnAllHosts>( ClientRPC::RPCDespawn( hostGameData.spaceshipID ), HostReplication::ResendUntilReplicated, sceneNode.handle );
						}
					}
				}
			}
		}
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct S_ProcessTimedOutPackets : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<ReliabilityLayer>();
		}
		static void Run( EcsWorld& /*_world*/, const EcsView& _view )
		{
			for( auto reliabilityLayerIt = _view.begin<ReliabilityLayer>(); reliabilityLayerIt != _view.end<ReliabilityLayer>(); ++reliabilityLayerIt )
			{
				ReliabilityLayer& reliabilityLayer = *reliabilityLayerIt;

				const double timoutTime = Time::ElapsedSinceStartup() - reliabilityLayer.timeoutDuration;

				//packets are sorted, so all timed out packets must be at front
				while( !reliabilityLayer.inFlightPackets.empty() )
				{
					ReliabilityLayer::InFlightPacket& inFlightPacket = reliabilityLayer.inFlightPackets.front();
					if( inFlightPacket.timeDispatch < timoutTime ) // packet timed out
					{
						inFlightPacket.onFailure.Emmit( inFlightPacket.tag );
						reliabilityLayer.inFlightPackets.pop();
					}
					else // no packets beyond could be timed out
					{
						break;
					}
				}
			}
		}
	};
}