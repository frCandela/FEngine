#include "scene/components/fanParticle.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void Particle::SetInfo( EcsComponentInfo& _info )
	{
		_info.name = "particle";
		_info.group = EngineGroups::Scene;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Particle::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		Particle& particle = static_cast<Particle&>( _component );
		particle.speed = glm::vec3(0,0,0);
		particle.position = glm::vec3( 0, 0, 0 );
		particle.color = Color::White;
		particle.durationLeft = 0.f;
	}
}