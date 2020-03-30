#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanSingletonComponent.hpp"

namespace fan
{
	struct Rigidbody;
	class EcsWorld;

	//================================================================================================================================
	// And empty singleton used to receive physics callbacks from the game
	//================================================================================================================================	
	struct CollisionManager : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );

		void OnBulletContact( Rigidbody* _other, btPersistentManifold* const& _manifold );

		EcsWorld* const world = nullptr;
	};
}