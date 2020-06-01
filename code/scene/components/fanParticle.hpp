#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "core/fanColor.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// A particle that disappears after some time
	// Is emmited from a particle emmiter
	//==============================================================================================================================================================
	struct Particle : public EcsComponent
	{
		ECS_COMPONENT( Particle )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );

		Color		color;
		glm::vec3	position;
		glm::vec3	speed;
		float		durationLeft;
	};
	static constexpr size_t sizeof_particle = sizeof( Particle );
}