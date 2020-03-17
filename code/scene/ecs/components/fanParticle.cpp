#include "scene/ecs/components/fanParticle.hpp"

namespace fan
{
	REGISTER_COMPONENT( Particle, "particle" );

	//================================================================================================================================
	//================================================================================================================================
	void Particle::SetInfo( ComponentInfo& _info )
	{
		_info.init = &Particle::Init;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Particle::Init( Component& _component )
	{
		Particle& particle = static_cast<Particle&>( _component );
		particle.speed = glm::vec3(0,0,0);
		particle.position = glm::vec3( 0, 0, 0 );
		particle.color = Color::White;
		particle.durationLeft = 0.f;
	}
}