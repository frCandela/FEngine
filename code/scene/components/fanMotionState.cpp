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
		_info.init = &MotionState::Init;
		_info.save = &MotionState::Save;
		_info.load = &MotionState::Load;
		_info.editorPath = "/";
		_info.name = "motion state";
	}

	//================================================================================================================================
	//================================================================================================================================
	void MotionState::Init( EcsWorld& _world, EcsComponent& _component )
	{
		// clear
		MotionState& motionState = static_cast<MotionState&>( _component );
		motionState.motionState = btDefaultMotionState();
	}
}