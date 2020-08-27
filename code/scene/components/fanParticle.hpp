#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "core/fanColor.hpp"

namespace fan
{
	//========================================================================================================
	// A particle that disappears after some time
	// Is emmited from a particle emmiter
	//========================================================================================================
	struct Particle : public EcsComponent
	{
		ECS_COMPONENT( Particle )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );

		Color     mColor;
		glm::vec3 mPosition;
		glm::vec3 mSpeed;
		float     mDurationLeft;
	};
}