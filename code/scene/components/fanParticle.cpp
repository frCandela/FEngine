#include "scene/components/fanParticle.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Particle::SetInfo( EcsComponentInfo& _info )
	{
		_info.mName  = "particle";
		_info.mGroup = EngineGroups::Scene;
	}

	//========================================================================================================
	//========================================================================================================
	void Particle::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		Particle& particle = static_cast<Particle&>( _component );
		particle.mSpeed        = glm::vec3( 0, 0, 0);
		particle.mPosition     = glm::vec3( 0, 0, 0 );
		particle.mColor        = Color::sWhite;
		particle.mDurationLeft = 0.f;
	}
}