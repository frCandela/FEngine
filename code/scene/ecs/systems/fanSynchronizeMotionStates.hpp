#include "scene/ecs/fanSystem.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

#include "ecs/components/fanRigidbody2.hpp"
#include "ecs/components/fanMotionState.hpp"
#include "ecs/components/fanTransform2.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// Updates the entity transform using the transform of the motion state of its rigidbody
	// Called after the physics update
	//==============================================================================================================================================================
	struct S_SynchronizeTransformFromMotionState : System
	{
	public:

		static Signature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<Transform2>()
 				| _world.GetSignature<MotionState>()
 				| _world.GetSignature<Rigidbody2>();
		}

		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
		{
			for( EntityID id : _entities )
			{
				// light data
				const MotionState& motionState = _world.GetComponent<MotionState>( id );
				const Rigidbody2& rigidbody = _world.GetComponent<Rigidbody2>( id );
				Transform2& transform = _world.GetComponent<Transform2>( id );

				

				if( rigidbody.rigidbody.getInvMass() <= 0.f ) { continue; }
				motionState.motionState.getWorldTransform( transform.transform );
			}
		}
	};

	//==============================================================================================================================================================
	// Updates rigidbody and motion state using the transform of the entity
	// Called before the physics update
	//==============================================================================================================================================================
	struct S_SynchronizeMotionStateFromTransform : System
	{
	public:

		static Signature GetSignature( const EcsWorld& _world )
		{
			return 
 				  _world.GetSignature<Transform2>()
 				| _world.GetSignature<MotionState>()
				| _world.GetSignature<Rigidbody2>();
		}

		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
		{
			for( EntityID id : _entities )
			{
				const Transform2& transform = _world.GetComponent<Transform2>( id );
				MotionState& motionState = _world.GetComponent<MotionState>( id );
				Rigidbody2& rigidbody = _world.GetComponent<Rigidbody2>( id );

 				rigidbody.rigidbody.setWorldTransform( transform.transform );
 				motionState.motionState.setWorldTransform( transform.transform );
			}
		}
	};
}