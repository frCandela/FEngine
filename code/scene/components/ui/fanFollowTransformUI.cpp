#include "scene/components/ui/fanFollowTransformUI.hpp"

#include "core/math/fanVector2.hpp"
#include "core/math/fanMathUtils.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "editor/fanModals.hpp"
#include "render/fanRendererDebug.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void FollowTransformUI::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::FOLLOW_TRANSFORM_UI16;
		_info.group = EngineGroups::SceneUI;
		_info.onGui =&FollowTransformUI::OnGui;
		_info.save = &FollowTransformUI::Save;
		_info.load = &FollowTransformUI::Load;
		_info.editorPath = "ui/";
		_info.name = "follow transform ui";
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransformUI::Init( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
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
	void FollowTransformUI::UpdateOffset( EcsWorld& _world, EcsEntity _entityID )
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
	void FollowTransformUI::OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
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
	void FollowTransformUI::Save( const EcsComponent& _component, Json& _json )
	{
		const FollowTransformUI& followTransform = static_cast<const FollowTransformUI&>( _component );
		Serializable::SaveComponentPtr( _json, "target_transform", followTransform.targetTransform );
		Serializable::SaveVec2( _json, "offset", ToBullet( followTransform.offset ) );
	}
	//================================================================================================================================
	//================================================================================================================================
	void FollowTransformUI::Load( EcsComponent& _component, const Json& _json )
	{
		FollowTransformUI& followTransform = static_cast<FollowTransformUI&>( _component );
		Serializable::LoadComponentPtr( _json, "target_transform", followTransform.targetTransform );
		btVector2 tmpVec2;
		Serializable::LoadVec2( _json, "offset", tmpVec2 );
		followTransform.offset = ToGLM( tmpVec2 );
	}
}