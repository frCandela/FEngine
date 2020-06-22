#include "ecs/fanEcsSystem.hpp"
#include "scene/components/fanExpirationTime.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// Kills the entities that expired
	//==============================================================================================================================================================
	struct S_UpdateExpirationTimes : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return	_world.GetSignature<ExpirationTime>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			for( auto expirationeIt = _view.begin<ExpirationTime>(); expirationeIt != _view.end<ExpirationTime>(); ++expirationeIt )
			{
				const EcsEntity entity = expirationeIt.GetEntity();
				ExpirationTime& expiration = *expirationeIt;

				expiration.duration -= _delta;
				if( expiration.duration < 0.f )
				{
					_world.Kill( entity );
				}
			}
		}
	};
}