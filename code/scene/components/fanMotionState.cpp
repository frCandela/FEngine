#include "scene/components/fanMotionState.hpp"
#include "scene/singletons/fanPhysicsWorld.hpp"
#include "core/ecs/fanEcsWorld.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void MotionState::SetInfo( EcsComponentInfo& _info )
	{
		_info.destroy     = &MotionState::Destroy;
		_info.save        = &MotionState::Save;
		_info.load        = &MotionState::Load;
	}

	//========================================================================================================
	//========================================================================================================
	void MotionState::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		// clear
		MotionState& motionState = static_cast<MotionState&>( _component );
		motionState.mMotionState = new btDefaultMotionState();
	}

	//========================================================================================================
	//========================================================================================================
	void MotionState::Destroy( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		MotionState& motionState = static_cast<MotionState&>( _component );
        fanAssert( motionState.mMotionState != nullptr );
		delete motionState.mMotionState;
		motionState.mMotionState = nullptr;
	}
}