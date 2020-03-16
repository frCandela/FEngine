#include "scene/ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Updates the entity transform using the transform of the motion state of its rigidbody
	// Called after the physics update
	//==============================================================================================================================================================
	struct S_SynchronizeTransformFromMotionState : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	// Updates rigidbody and motion state using the transform of the entity
	// Called before the physics update
	//==============================================================================================================================================================
	struct S_SynchronizeMotionStateFromTransform : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};
}