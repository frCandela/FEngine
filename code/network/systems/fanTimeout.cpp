#include "network/systems/fanTimeout.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanTime.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "network/singletonComponents/fanHostManager.hpp"
#include "network/components/fanHostConnection.hpp"
#include "network/components/fanReliabilityLayer.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_ProcessTimedOutPackets::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<ReliabilityLayer>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ProcessTimedOutPackets::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		for( EntityID entityID : _entities )
		{
			ReliabilityLayer& reliabilityLayer = _world.GetComponent<ReliabilityLayer>( entityID );
			const double timoutTime = Time::Get().ElapsedSinceStartup() - reliabilityLayer.timeoutDuration;

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

	//================================================================================================================================
	//================================================================================================================================
	Signature S_DetectHostTimout::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<HostConnection>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_DetectHostTimout::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		HostManager& hostManager = _world.GetSingletonComponent<HostManager>();
		const double currentTime = Time::Get().ElapsedSinceStartup();
		for( EntityID entityID : _entities )
		{
			HostConnection& connection = _world.GetComponent<HostConnection>( entityID );

			//packets are sorted, so all timed out packets must be at front
			if( connection.state == HostConnection::Connected )
			{				
				if( connection.lastResponseTime + connection.timeoutDelay < currentTime )
				{
					Debug::Log() << "client timeout " << Debug::Endl();
					SceneNode& sceneNode = _world.GetComponent<SceneNode>( entityID );
					hostManager.DeleteHost( sceneNode.handle );
				}
			}
		}
	}
}