#include "core/ecs/fanEcsSystem.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "network/singletons/fanHostManager.hpp"
#include "network/components/fanHostConnection.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "network/components/fanHostGameData.hpp"
#include "network/components/fanClientRPC.hpp"
#include "network/systems/fanHostReplication.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	struct SDetectHostTimout : EcsSystem
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
            for( ; hostConnectionit != _view.end<HostConnection>();
                   ++hostConnectionit, ++sceneNodeIt, ++hostGameDataIt )
			{
				const HostConnection& connection = *hostConnectionit;
				const SceneNode& sceneNode = *sceneNodeIt;

				// packets are sorted, so all timed out packets must be at front
				if( connection.mState == HostConnection::Connected )
				{
					if( connection.mLastResponseTime + connection.mTimeoutDelay < currentTime )
					{
						Debug::Log() << "client timeout " << Debug::Endl();

						hostManager.DeleteHost( _world, sceneNode.mHandle );
						const HostGameData& hostGameData = *hostGameDataIt;
						if( hostGameData.mSpaceshipID != 0 )
						{
                            _world.Run<SReplicateOnAllHosts>(
                                    ClientRPC::RPCDespawn( hostGameData.mSpaceshipID ),
                                    HostReplication::ResendUntilReplicated,
                                    sceneNode.mHandle );
						}
					}
				}
			}
		}
	};

	//========================================================================================================
	//========================================================================================================
	struct SProcessTimedOutPackets : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<ReliabilityLayer>();
		}
		static void Run( EcsWorld& /*_world*/, const EcsView& _view )
        {
            for( auto reliabilityLayerIt = _view.begin<ReliabilityLayer>();
                 reliabilityLayerIt != _view.end<ReliabilityLayer>();
                 ++reliabilityLayerIt )
			{
				ReliabilityLayer& reliabilityLayer = *reliabilityLayerIt;

				const double timoutTime = Time::ElapsedSinceStartup() - reliabilityLayer.sTimeoutDuration;

				//packets are sorted, so all timed out packets must be at front
				while( !reliabilityLayer.mInFlightPackets.empty() )
				{
					ReliabilityLayer::InFlightPacket& inFlightPacket = reliabilityLayer.mInFlightPackets.front();
					if( inFlightPacket.mTimeDispatch < timoutTime ) // packet timed out
					{
						inFlightPacket.mOnFailure.Emmit( inFlightPacket.mTag );
						reliabilityLayer.mInFlightPackets.pop();
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