#include "scene/systems/fanUpdateTimers.hpp"

#include "scene/components/fanExpirationTime.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_UpdateExpirationTimes::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<ExpirationTime>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateExpirationTimes::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		for( auto expirationeIt = _view.begin<ExpirationTime>(); expirationeIt != _view.end<ExpirationTime>(); ++expirationeIt )
		{
			const EcsEntity entity = expirationeIt.Entity();
			ExpirationTime& expiration = *expirationeIt;

			expiration.duration -= _delta;
			if( expiration.duration < 0.f )
			{
				_world.Kill( entity );
			}
		}
	}
}