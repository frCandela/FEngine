#include "scene/components/fanFollowTransform.hpp"
#include "scene/systems/fanUpdateTransforms.hpp"
#include "scene/singletons/fanRenderDebug.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void FollowTransform::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::FollowTransform16;
		_info.mGroup      = EngineGroups::Scene;
		_info.onGui       = &FollowTransform::OnGui;
		_info.save        = &FollowTransform::Save;
		_info.load        = &FollowTransform::Load;
		_info.mEditorPath = "/";
		_info.mName       = "follow transform";
	}

	//========================================================================================================
	//========================================================================================================
	void FollowTransform::Init( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		// clear
		FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
		followTransform.mTargetTransform.Init( _world );
		followTransform.mLocalTransform = btTransform::getIdentity();
		followTransform.mLocked         = true;
	}

	//========================================================================================================
	// calculates the new local transform from the follower & target transforms
	//========================================================================================================
	void FollowTransform::UpdateLocalTransform( EcsWorld& _world, EcsEntity _entityID )
	{
		assert( _world.HasComponent<FollowTransform>( _entityID ) );

		FollowTransform& follower = _world.GetComponent<FollowTransform>( _entityID );

		if( follower.mTargetTransform != nullptr && _world.HasComponent<Transform>( _entityID ) )
		{
			Transform& target = *follower.mTargetTransform;
			Transform& follow = _world.GetComponent<Transform>( _entityID );
            follower.mLocalTransform = SInitFollowTransforms::GetLocalTransform( target.mTransform,
                                                                                 follow.mTransform );
		}
		else
		{
			follower.mLocalTransform = btTransform::getIdentity();
		}
	}

	//========================================================================================================
	//========================================================================================================
	void FollowTransform::Save( const EcsComponent& _component, Json& _json )
	{
		const FollowTransform& followTransform = static_cast<const FollowTransform&>( _component );
		Serializable::SaveComponentPtr( _json, "target_transform", followTransform.mTargetTransform );
		Serializable::SaveVec3( _json, "local_position", followTransform.mLocalTransform.getOrigin() );
		Serializable::SaveQuat( _json, "local_rotation", followTransform.mLocalTransform.getRotation() );
	}

	//========================================================================================================
	//========================================================================================================
	void FollowTransform::Load( EcsComponent& _component, const Json& _json )
	{
		FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
		Serializable::LoadComponentPtr( _json, "target_transform", followTransform.mTargetTransform );
		btVector3 tmpVec;
		btQuaternion tmpQuat;
		Serializable::LoadVec3( _json, "local_position", tmpVec );
		Serializable::LoadQuat( _json, "local_rotation", tmpQuat );
		followTransform.mLocalTransform.setOrigin( tmpVec );
		followTransform.mLocalTransform.setRotation( tmpQuat );
	}

    //========================================================================================================
    //========================================================================================================
    void FollowTransform::OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
    {
        FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
        if( ImGui::FanComponent<Transform>( "target transform", followTransform.mTargetTransform ) )
        {
            UpdateLocalTransform( _world, _entityID );
        }

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );

        enum ImGui::IconType icon = followTransform.mLocked ? ImGui::LockClosed16 : ImGui::LockOpen16;
        if( ImGui::ButtonIcon( icon, { 16,16 } ) )
        {
            followTransform.mLocked = !followTransform.mLocked;
            UpdateLocalTransform( _world, _entityID );
        }
        ImGui::SameLine();
        ImGui::Text( "lock transform " );
        ImGui::SameLine();
        ImGui::FanShowHelpMarker( "press L to toggle" );

        // draw the local transform offset
        if( followTransform.mTargetTransform != nullptr && _world.HasComponent<Transform>( _entityID ) )
        {
            btTransform& target = followTransform.mTargetTransform->mTransform;
            btTransform& local = followTransform.mLocalTransform;
            btTransform t = target * local;

            _world.GetSingleton<RenderDebug>().DebugLine( target.getOrigin(), t.getOrigin(), Color::sRed );
        }
    }
}