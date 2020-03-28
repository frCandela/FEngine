#include "scene/components/fanFollowTransform.hpp"

#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	REGISTER_COMPONENT( FollowTransform, "follow_transform" );

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::TRANSFORM16;
		_info.init = &FollowTransform::Init;
		_info.onGui = &FollowTransform::OnGui;
		_info.save = &FollowTransform::Save;
		_info.load = &FollowTransform::Load;
		_info.editorPath = "/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::Init( EcsWorld& _world, Component& _component )
	{
		// clear
		FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
		followTransform.targetTransform.Init( _world );
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::OnGui( Component& _component )
	{
		FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
		ImGui::FanComponent( "target transform", followTransform.targetTransform );
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::Save( const Component& _component, Json& _json )
	{
		const FollowTransform& followTransform = static_cast<const FollowTransform&>( _component );
		Serializable::SaveComponentPtr( _json, "target_transform", followTransform.targetTransform );
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::Load( Component& _component, const Json& _json )
	{
		FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
		Serializable::LoadComponentPtr( _json, "target_transform", followTransform.targetTransform );
	}
}