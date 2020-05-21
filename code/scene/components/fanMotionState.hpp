#pragma  once

#include "ecs/fanEcsComponent.hpp"

#include "bullet/btBulletDynamicsCommon.h"

namespace fan
{
	//==============================================================================================================================================================
	// MotionState is used synchronizes physics transforms with graphics transforms
	//==============================================================================================================================================================
	struct MotionState : public EcsComponent
	{
		ECS_COMPONENT( MotionState )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json ) {}
		static void Load( EcsComponent& _component, const Json& _json ) {}

		btDefaultMotionState motionState;
	};
	static constexpr size_t sizeof_motionState = sizeof( MotionState );
}