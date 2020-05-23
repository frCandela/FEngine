#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "bullet/btBulletDynamicsCommon.h"

namespace fan
{
	//==============================================================================================================================================================
	// sphere physics shape
	//==============================================================================================================================================================
	struct SphereShape : public EcsComponent
	{
		ECS_COMPONENT( SphereShape )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void Destroy( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		void  SetRadius( const float _radius );
		float GetRadius() const;

		btSphereShape* sphereShape;
	};
	static constexpr size_t sizeof_sphereShape = sizeof( SphereShape );
}