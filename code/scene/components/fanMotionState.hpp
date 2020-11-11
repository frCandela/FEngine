#pragma  once

#include "core/ecs/fanEcsComponent.hpp"
#include "fanDisableWarnings.hpp"
WARNINGS_BULLET_PUSH()
#include "bullet/btBulletDynamicsCommon.h"
WARNINGS_POP()

namespace fan
{
	//========================================================================================================
	// MotionState is used synchronizes physics transforms with graphics transforms
	//========================================================================================================
	struct MotionState : public EcsComponent
	{
		ECS_COMPONENT( MotionState )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Destroy( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Save( const EcsComponent& /*_component*/, Json& /*_json*/ ) {}
		static void Load( EcsComponent& /*_component*/, const Json& /*_json*/ ) {}
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );

		btDefaultMotionState* mMotionState;
	};
}