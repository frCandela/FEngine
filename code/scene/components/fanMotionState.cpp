#include "scene/components/fanMotionState.hpp"
#include "scene/singletons/fanPhysicsWorld.hpp"
#include "core/ecs/fanEcsWorld.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void MotionState::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Rigidbody16;
		_info.mGroup      = EngineGroups::ScenePhysics;
		_info.destroy     = &MotionState::Destroy;
		_info.save        = &MotionState::Save;
		_info.load        = &MotionState::Load;
		_info.onGui       = &MotionState::OnGui;
		_info.mEditorPath = "/";
		_info.mName       = "motion state";
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

	//========================================================================================================
	//========================================================================================================
	void MotionState::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& /*_component*/ ){}
}