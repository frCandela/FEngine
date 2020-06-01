#include "scene/components/fanMotionState.hpp"

#include "render/fanRenderSerializable.hpp"
#include "scene/singletonComponents/fanPhysicsWorld.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void MotionState::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::RIGIDBODY16;
		_info.destroy = &MotionState::Destroy;
		_info.save = &MotionState::Save;
		_info.load = &MotionState::Load;
		_info.editorPath = "/";
		_info.name = "motion state";
	}

	//================================================================================================================================
	//================================================================================================================================
	void MotionState::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		// clear
		MotionState& motionState = static_cast<MotionState&>( _component );
		motionState.motionState = new btDefaultMotionState();
	}

	//================================================================================================================================
	//================================================================================================================================
	void MotionState::Destroy( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		MotionState& motionState = static_cast<MotionState&>( _component );
		assert( motionState.motionState != nullptr );
		delete motionState.motionState;
		motionState.motionState = nullptr;
	}
}