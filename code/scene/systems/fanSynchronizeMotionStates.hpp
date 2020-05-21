#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Updates the entity transform using the transform of the motion state of its rigidbody
	// Called after the physics update
	//==============================================================================================================================================================
	struct S_SynchronizeTransformFromMotionState : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	// Updates rigidbody and motion state using the transform of the entity
	// Called before the physics update
	//==============================================================================================================================================================
	struct S_SynchronizeMotionStateFromTransform : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};
}