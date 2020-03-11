#include "scene/ecs/components/fanMotionState.hpp"

#include "render/fanRenderSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( MotionState, "motion_state" );

	//================================================================================================================================
	//================================================================================================================================
	void MotionState::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::RIGIDBODY16;
		_info.clear = &MotionState::Clear;
		_info.editorPath = "";
	}

	//================================================================================================================================
	//================================================================================================================================
	void MotionState::Clear( ecComponent& _motionState )
	{
		MotionState& motionState = static_cast<MotionState&>( _motionState );
		motionState.motionState = btDefaultMotionState();
	}
}