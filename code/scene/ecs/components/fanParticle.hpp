#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEcComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct Particle : public ecComponent
	{
		DECLARE_COMPONENT( Particle )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( ecComponent& _component );

		Color		color;
		glm::vec3	position;
		glm::vec3	speed;
		float		durationLeft;
	};
	static constexpr size_t sizeof_particle = sizeof( Particle );
}