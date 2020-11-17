#pragma  once

#include "core/ecs/fanEcsComponent.hpp"
#include "fanDisableWarnings.hpp"
WARNINGS_BULLET_PUSH()
#include "bullet/btBulletDynamicsCommon.h"
WARNINGS_POP()

namespace fan
{
	//========================================================================================================
	// sphere physics shape
	//========================================================================================================
	struct SphereShape : public EcsComponent
	{
		ECS_COMPONENT( SphereShape )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Destroy( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		void  SetRadius( const float _radius );
		float GetRadius() const;

		btSphereShape* mSphereShape;
	};
}