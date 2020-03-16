#include "scene/ecs/systems/fanSynchronizeMotionStates.hpp"

#include "scene/ecs/components/fanRigidbody2.hpp"
#include "scene/ecs/components/fanMotionState.hpp"
#include "scene/ecs/components/fanTransform2.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_SynchronizeTransformFromMotionState::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform2>()
			| _world.GetSignature<MotionState>()
			| _world.GetSignature<Rigidbody2>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_SynchronizeTransformFromMotionState::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
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

	//================================================================================================================================
	//================================================================================================================================
	Signature S_SynchronizeMotionStateFromTransform::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform2>()
			| _world.GetSignature<MotionState>()
			| _world.GetSignature<Rigidbody2>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_SynchronizeMotionStateFromTransform::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
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
}