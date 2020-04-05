#include "scene/systems/fanSynchronizeMotionStates.hpp"

#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanTransform.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_SynchronizeTransformFromMotionState::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform>()
			| _world.GetSignature<MotionState>()
			| _world.GetSignature<Rigidbody>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_SynchronizeTransformFromMotionState::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		for( EntityID id : _entities )
		{
			// light data
			const MotionState& motionState = _world.GetComponent<MotionState>( id );
			const Rigidbody& rigidbody = _world.GetComponent<Rigidbody>( id );
			Transform& transform = _world.GetComponent<Transform>( id );



			if( rigidbody.rigidbody.getInvMass() <= 0.f ) { continue; }
			motionState.motionState.getWorldTransform( transform.transform );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Signature S_SynchronizeMotionStateFromTransform::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform>()
			| _world.GetSignature<MotionState>()
			| _world.GetSignature<Rigidbody>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_SynchronizeMotionStateFromTransform::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		for( EntityID id : _entities )
		{
			const Transform& transform = _world.GetComponent<Transform>( id );
			MotionState& motionState = _world.GetComponent<MotionState>( id );
			Rigidbody& rigidbody = _world.GetComponent<Rigidbody>( id );

			rigidbody.rigidbody.setWorldTransform( transform.transform );
			motionState.motionState.setWorldTransform( transform.transform );
		}
	}
}