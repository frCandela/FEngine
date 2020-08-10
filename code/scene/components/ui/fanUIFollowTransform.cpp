#include "scene/components/ui/fanUIFollowTransform.hpp"

#include "core/math/fanVector2.hpp"
#include "core/math/fanMathUtils.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UIFollowTransform::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::FOLLOW_TRANSFORM_UI16;
		_info.group = EngineGroups::SceneUI;
		_info.onGui =&UIFollowTransform::OnGui;
		_info.save = &UIFollowTransform::Save;
		_info.load = &UIFollowTransform::Load;
		_info.editorPath = "ui/";
		_info.name = "ui follow transform";
	}

	//========================================================================================================
	//========================================================================================================
	void UIFollowTransform::Init( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		// clear
		UIFollowTransform& followTransform = static_cast<UIFollowTransform&>( _component );
		followTransform.mTarget.Init( _world );
		followTransform.mOffset = { 0, 0 };
		followTransform.mlocked = true;
	}

	//========================================================================================================
	// calculates the new local transform from the follower & target transforms
	//========================================================================================================
	void UIFollowTransform::UpdateOffset( EcsWorld& _world, EcsEntity _entityID )
	{
		assert( _world.HasComponent<UIFollowTransform>( _entityID ) );

		UIFollowTransform& follower = _world.GetComponent<UIFollowTransform>( _entityID );

		if( follower.mTarget != nullptr && _world.HasComponent<UITransform>( _entityID ) )
		{
			UITransform& target = *follower.mTarget;
			UITransform& follow = _world.GetComponent<UITransform>( _entityID );
			follower.mOffset = follow.mPosition - target.mPosition;
		}
		else
		{
			follower.mOffset = { 0, 0 };
		}
	}

	//========================================================================================================
	//========================================================================================================
	void UIFollowTransform::Save( const EcsComponent& _component, Json& _json )
	{
		const UIFollowTransform& followTransform = static_cast<const UIFollowTransform&>( _component );
		Serializable::SaveComponentPtr( _json, "target_transform", followTransform.mTarget );
		Serializable::SaveIVec2( _json, "offset", followTransform.mOffset );
	}
	//========================================================================================================
	//========================================================================================================
	void UIFollowTransform::Load( EcsComponent& _component, const Json& _json )
	{
		UIFollowTransform& followTransform = static_cast<UIFollowTransform&>( _component );
		Serializable::LoadComponentPtr( _json, "target_transform", followTransform.mTarget );
		Serializable::LoadIVec2( _json, "offset", followTransform.mOffset );
	}

    //========================================================================================================
    //========================================================================================================
    void UIFollowTransform::OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
    {
        UIFollowTransform& followTransform = static_cast<UIFollowTransform&>( _component );
        if( ImGui::FanComponent<UITransform>( "target transform", followTransform.mTarget ) )
        {
            UpdateOffset( _world, _entityID );
        }

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );

        enum ImGui::IconType icon = followTransform.mlocked ? ImGui::LOCK_CLOSED16 : ImGui::LOCK_OPEN16;
        if( ImGui::ButtonIcon( icon, { 16,16 } ) )
        {
            followTransform.mlocked = !followTransform.mlocked;
            UpdateOffset( _world, _entityID );
        }
        ImGui::SameLine();
        ImGui::Text( "lock transform " );
        ImGui::SameLine();
        ImGui::FanShowHelpMarker( "press L to toggle" );
    }
}