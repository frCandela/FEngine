#pragma once

#include "ecs/fanEcsComponent.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// round object that gravitates around the sun at a certain speed
	//==============================================================================================================================================================
	struct Planet : public EcsComponent
	{
		ECS_COMPONENT( Planet )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		float speed;
		float radius;
		float phase;
	};
	static constexpr size_t sizeof_planet = sizeof( Planet );
}