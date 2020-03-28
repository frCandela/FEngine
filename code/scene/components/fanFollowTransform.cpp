#include "scene/components/fanFollowTransform.hpp"

#include "render/fanRenderSerializable.hpp"
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
	void FollowTransform::Init( EcsWorld&, Component& _component )
	{
		// clear
		FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::OnGui( Component& _component )
	{
		FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::Save( const Component& _component, Json& _json )
	{
		const FollowTransform& followTransform = static_cast<const FollowTransform&>( _component );
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::Load( Component& _component, const Json& _json )
	{
		FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
	}
}