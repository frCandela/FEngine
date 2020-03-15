#include "scene/ecs/components/fanMotionState.hpp"

#include "render/fanRenderSerializable.hpp"
#include "scene/ecs/fanEcsWorld.hpp"
#include "scene/ecs/singletonComponents/fanPhysicsWorld.hpp"

namespace fan
{
	REGISTER_COMPONENT( MotionState, "motion_state" );

	//================================================================================================================================
	//================================================================================================================================
	void MotionState::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::RIGIDBODY16;
		_info.init = &MotionState::Init;
		_info.save = &MotionState::Save;
		_info.load = &MotionState::Load;
		_info.editorPath = "/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void MotionState::Init( ecComponent& _component )
	{
		// clear
		MotionState& motionState = static_cast<MotionState&>( _component );
		motionState.motionState = btDefaultMotionState();
	}
}