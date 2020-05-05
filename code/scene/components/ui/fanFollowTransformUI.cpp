#include "scene/components/ui/fanFollowTransformUI.hpp"

#include "core/math/fanVector2.hpp"
#include "core/math/fanMathUtils.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "editor/fanModals.hpp"
#include "render/fanRendererDebug.hpp"

namespace fan
{
	REGISTER_COMPONENT( FollowTransformUI, "follow transform ui" );

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransformUI::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::UI_TRANSFORM16;
		_info.init = &FollowTransformUI::Init;
		_info.onGui =&FollowTransformUI::OnGui;
		_info.save = &FollowTransformUI::Save;
		_info.load = &FollowTransformUI::Load;
		_info.editorPath = "ui/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransformUI::Init( EcsWorld& _world, Component& _component )
	{
		// clear
		FollowTransformUI& followTransform = static_cast<FollowTransformUI&>( _component );
		followTransform.targetTransform.Init( _world );
		followTransform.offset = { 0,0 };
		followTransform.locked = true;
	}

	//================================================================================================================================
	// calculates the new local transform from the follower & target transforms
	//================================================================================================================================
	void FollowTransformUI::UpdateOffset( EcsWorld& _world, EntityID _entityID )
	{
		assert( _world.HasComponent<FollowTransformUI>( _entityID ) );

		FollowTransformUI& follower = _world.GetComponent<FollowTransformUI>( _entityID );

		if( follower.targetTransform != nullptr && _world.HasComponent<TransformUI>( _entityID ) )
		{
			TransformUI& target = *follower.targetTransform;
			TransformUI& follow = _world.GetComponent<TransformUI>( _entityID );
			follower.offset = follow.position - target.position;
		}
		else
		{
			follower.offset = { 0,0 };
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransformUI::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		FollowTransformUI& followTransform = static_cast<FollowTransformUI&>( _component );
		if( ImGui::FanComponent<TransformUI>( "target transform", followTransform.targetTransform ) )
		{
			UpdateOffset( _world, _entityID );
		}

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );

		enum ImGui::IconType icon = followTransform.locked ? ImGui::LOCK_CLOSED16 : ImGui::LOCK_OPEN16;
		if( ImGui::ButtonIcon( icon, { 16,16 } ) )
		{
			followTransform.locked = !followTransform.locked;
			UpdateOffset( _world, _entityID );
		}
		ImGui::SameLine();
		ImGui::Text( "lock transform " );
		ImGui::SameLine();
		ImGui::FanShowHelpMarker( "press L to toggle" );
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransformUI::Save( const Component& _component, Json& _json )
	{
		const FollowTransformUI& followTransform = static_cast<const FollowTransformUI&>( _component );
		Serializable::SaveComponentPtr( _json, "target_transform", followTransform.targetTransform );
		Serializable::SaveVec2( _json, "offset", ToBullet( followTransform.offset ) );
	}
	//================================================================================================================================
	//================================================================================================================================
	void FollowTransformUI::Load( Component& _component, const Json& _json )
	{
		FollowTransformUI& followTransform = static_cast<FollowTransformUI&>( _component );
		Serializable::LoadComponentPtr( _json, "target_transform", followTransform.targetTransform );
		btVector2 tmpVec2;
		Serializable::LoadVec2( _json, "offset", tmpVec2 );
		followTransform.offset = ToGLM( tmpVec2 );
	}
}