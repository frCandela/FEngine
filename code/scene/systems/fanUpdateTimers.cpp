#include "scene/systems/fanUpdateTimers.hpp"

#include "scene/components/fanExpirationTime.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_UpdateExpirationTimes::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<ExpirationTime>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateExpirationTimes::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		for( EntityID entityID : _entities )
		{
			ExpirationTime& expiration = _world.GetComponent<ExpirationTime>( entityID );

			expiration.duration -= _delta;
			if( expiration.duration < 0.f )
			{
				_world.KillEntity( entityID );
			}
		}
	}
}