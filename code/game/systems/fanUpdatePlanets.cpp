#include "game/systems/fanUpdatePlanets.hpp"

#include "scene/fanSceneTags.hpp"
#include "scene/components/fanTransform.hpp"
#include "game/components/fanPlanet.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_MovePlanets::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform>() |
			_world.GetSignature<Planet>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_MovePlanets::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		for( EntityID entityID : _entities )
		{
			Transform& transform = _world.GetComponent<Transform>( entityID );
			Planet& planet = _world.GetComponent<Planet>( entityID );

			planet.timeAccumulator += _delta;
			float const time = -planet.speed * planet.timeAccumulator;
			btVector3 position( std::cosf( time + planet.phase ), 0, std::sinf( time + planet.phase ) );

			if( std::abs( time ) > SIMD_2_PI ) { planet.timeAccumulator -= SIMD_2_PI / std::abs( planet.speed ); }
			transform.SetPosition( /*parentTransform.getOrigin()*/ planet.radius * position );

			_world.AddTag<tag_boundsOutdated>( entityID );
		}
	}
}