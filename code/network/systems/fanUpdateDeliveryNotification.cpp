#include "network/systems/fanUpdateDeliveryNotification.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "network/components/fanHostDeliveryNotification.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_ProcessTimedOutPackets::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<HostDeliveryNotification>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ProcessTimedOutPackets::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		for( EntityID entityID : _entities )
		{
			HostDeliveryNotification deliveryNotifications = _world.GetComponent<HostDeliveryNotification>( entityID );

			//packets are sorted, so all timed out packets must be at front
			while( !deliveryNotifications.inFlightPackets.empty() )
			{
				HostDeliveryNotification::InFlightPacket& inFlightPacket = deliveryNotifications.inFlightPackets.front();
				if( inFlightPacket.timeDispatch < deliveryNotifications.timeoutDuration ) // packet timed out
				{
					inFlightPacket.onFailure.Emmit( inFlightPacket.tag );
					deliveryNotifications.inFlightPackets.pop();
				}
				else // no packets beyond could be timed out
				{
					break;
				}
			}
		}
	}
}