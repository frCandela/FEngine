#include "ecs/fanEcsSystem.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanTransform.hpp"

namespace fan
{
	//========================================================================================================
	// Updates the entity transform using the transform of the motion state of its rigidbody
	// Called after the physics update
	//========================================================================================================
	struct SSynchronizeTransformFromMotionState : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<Transform>()
				| _world.GetSignature<MotionState>()
				| _world.GetSignature<Rigidbody>();
		}

		static void Run( EcsWorld& /*_world*/, const EcsView& _view )
		{
			auto motionStateIt = _view.begin<MotionState>();
			auto rigidbodyIt = _view.begin<Rigidbody>();
			auto transformIt = _view.begin<Transform>();
			for( ; transformIt != _view.end<Transform>(); ++motionStateIt, ++rigidbodyIt, ++transformIt )
			{
				const MotionState& motionState = *motionStateIt;
				const Rigidbody& rigidbody = *rigidbodyIt;
				Transform& transform = *transformIt;

				if( rigidbody.mRigidbody->getInvMass() <= 0.f ) { continue; }
				motionState.mMotionState->getWorldTransform( transform.mTransform );
			}
		}
	};

	//========================================================================================================
	// Updates rigidbody and motion state using the transform of the entity
	// Called before the physics update
	//========================================================================================================
	struct SSynchronizeMotionStateFromTransform : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<Transform>()
				| _world.GetSignature<MotionState>()
				| _world.GetSignature<Rigidbody>();
		}

		static void Run( EcsWorld& /*_world*/, const EcsView& _view )
		{
			auto motionStateIt = _view.begin<MotionState>();
			auto rigidbodyIt = _view.begin<Rigidbody>();
			auto transformIt = _view.begin<Transform>();
			for( ; transformIt != _view.end<Transform>(); ++motionStateIt, ++rigidbodyIt, ++transformIt )
			{
				MotionState& motionState = *motionStateIt;
				Rigidbody& rigidbody = *rigidbodyIt;
				const Transform& transform = *transformIt;

				rigidbody.mRigidbody->setWorldTransform( transform.mTransform );
				motionState.mMotionState->setWorldTransform( transform.mTransform );
			}
		}
	};
}