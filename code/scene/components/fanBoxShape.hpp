#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "core/fanBulletWarnings.hpp"
BULLET_PUSH()
#include "bullet/btBulletDynamicsCommon.h"
BULLET_POP()
namespace fan
{
	//==============================================================================================================================================================
	// physics box collision shape
	//==============================================================================================================================================================
	struct BoxShape : public EcsComponent
	{
		ECS_COMPONENT( BoxShape )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Destroy( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );
		
		void	  SetScaling( const btVector3 _scaling );
		btVector3 GetScaling() const;

		btBoxShape* boxShape;
	};
	static constexpr size_t sizeof_boxShape = sizeof( BoxShape );
}