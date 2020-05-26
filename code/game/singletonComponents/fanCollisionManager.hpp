#pragma once

#include "bullet/btBulletDynamicsCommon.h"
#include "ecs/fanEcsSingleton.hpp"

namespace fan
{
	struct Rigidbody;
	class EcsWorld;

	//================================================================================================================================
	// And empty singleton used to receive physics callbacks from the game
	//================================================================================================================================	
	struct CollisionManager : public EcsSingleton
	{
		ECS_SINGLETON( CollisionManager )
	public:
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );

		void OnBulletContact( Rigidbody& _bulletBody, Rigidbody& _otherBody, btPersistentManifold* const& _manifold );
		void OnSpaceShipContact( Rigidbody& _spaceshipBody, Rigidbody& _otherBody, btPersistentManifold* const& _manifold );

		EcsWorld* const world = nullptr;
	};
}