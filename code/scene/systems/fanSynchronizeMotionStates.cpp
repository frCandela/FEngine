#include "scene/systems/fanSynchronizeMotionStates.hpp"

#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanTransform.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_SynchronizeTransformFromMotionState::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform>()
			| _world.GetSignature<MotionState>()
			| _world.GetSignature<Rigidbody>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_SynchronizeTransformFromMotionState::Run( EcsWorld& /*_world*/, const EcsView& _view )
	{
		auto motionStateIt = _view.begin<MotionState>();
		auto rigidbodyIt = _view.begin<Rigidbody>();
		auto transformIt = _view.begin<Transform>();
		for( ; transformIt != _view.end<Transform>(); ++motionStateIt, ++rigidbodyIt, ++transformIt )
		{
			const MotionState& motionState = *motionStateIt;
			const Rigidbody& rigidbody = *rigidbodyIt;
			Transform& transform = *transformIt;

			if( rigidbody.rigidbody->getInvMass() <= 0.f ) { continue; }
			motionState.motionState->getWorldTransform( transform.transform );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_SynchronizeMotionStateFromTransform::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform>()
			| _world.GetSignature<MotionState>()
			| _world.GetSignature<Rigidbody>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_SynchronizeMotionStateFromTransform::Run( EcsWorld& /*_world*/, const EcsView& _view )
	{
		auto motionStateIt = _view.begin<MotionState>();
		auto rigidbodyIt = _view.begin<Rigidbody>();
		auto transformIt = _view.begin<Transform>();
		for( ; transformIt != _view.end<Transform>(); ++motionStateIt, ++rigidbodyIt, ++transformIt )
		{
			MotionState& motionState = *motionStateIt;
			Rigidbody& rigidbody = *rigidbodyIt;
			const Transform& transform = *transformIt;

			rigidbody.rigidbody->setWorldTransform( transform.transform );
			motionState.motionState->setWorldTransform( transform.transform );
		}
	}
}