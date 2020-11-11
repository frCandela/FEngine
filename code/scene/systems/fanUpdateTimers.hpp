#include "core/ecs/fanEcsSystem.hpp"
#include "scene/components/fanExpirationTime.hpp"

namespace fan
{
	//========================================================================================================
	// Kills the entities that expired
	//========================================================================================================
	struct SUpdateExpirationTimes : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return	_world.GetSignature<ExpirationTime>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

            for( auto expirationIt = _view.begin<ExpirationTime>();
                 expirationIt != _view.end<ExpirationTime>();
                 ++expirationIt )
			{
				const EcsEntity entity = expirationIt.GetEntity();
				ExpirationTime& expiration = *expirationIt;

				expiration.mDuration -= _delta;
				if( expiration.mDuration < 0.f )
				{
					_world.Kill( entity );
				}
			}
		}
	};
}