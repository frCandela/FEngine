#include "scene/components/fanParticle.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void Particle::SetInfo( EcsComponentInfo& _info )
	{
		_info.init = &Particle::Init;
		_info.name = "particle";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Particle::Init( EcsWorld& _world, EcsComponent& _component )
	{
		Particle& particle = static_cast<Particle&>( _component );
		particle.speed = glm::vec3(0,0,0);
		particle.position = glm::vec3( 0, 0, 0 );
		particle.color = Color::White;
		particle.durationLeft = 0.f;
	}
}