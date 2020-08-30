#include "scene/components/fanFollowTransform.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/systems/fanUpdateTransforms.hpp"
#include "scene/singletons/fanRenderDebug.hpp"
#include "scene/fanSceneSerializable.hpp"
#include "scene/singletons/fanScene.hpp"
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
	void FollowTransform::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		// clear
		FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
		followTransform.mLocalTransform = btTransform::getIdentity();
		followTransform.mLocked         = false;
	}

	//========================================================================================================
	// calculates the new local transform from the follower & target transforms
	//========================================================================================================
    void FollowTransform::UpdateLocalTransform( FollowTransform& _followTransform,
                                                Transform& _transform,
                                                SceneNode& _sceneNode )
	{
	    fanAssert( _sceneNode.mParentHandle != 0 );
	    EcsWorld& world = * _sceneNode.mScene->mWorld;
	    EcsEntity parentEntity = world.GetEntity( _sceneNode.mParentHandle );
	    Transform * parentTransform = world.SafeGetComponent<Transform>( parentEntity );

		if( parentTransform != nullptr )
		{
            _followTransform.mLocalTransform = SInitFollowTransforms::GetLocalTransform(
                    parentTransform->mTransform,
                    _transform.mTransform );
		}
		else
		{
            _followTransform.mLocalTransform = btTransform::getIdentity();
		}
	}

	//========================================================================================================
	//========================================================================================================
	void FollowTransform::Save( const EcsComponent& _component, Json& _json )
	{
		const FollowTransform& followTransform = static_cast<const FollowTransform&>( _component );
		Serializable::SaveVec3( _json, "local_position", followTransform.mLocalTransform.getOrigin() );
		Serializable::SaveQuat( _json, "local_rotation", followTransform.mLocalTransform.getRotation() );
	}

	//========================================================================================================
	//========================================================================================================
	void FollowTransform::Load( EcsComponent& _component, const Json& _json )
	{
		FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
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
        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );

        enum ImGui::IconType icon = followTransform.mLocked ? ImGui::LockClosed16 : ImGui::LockOpen16;
        if( ImGui::ButtonIcon( icon, { 16,16 } ) )
        {
            followTransform.mLocked = !followTransform.mLocked;

            Transform * transform = _world.SafeGetComponent<Transform>( _entityID );
            SceneNode * sceneNode = _world.SafeGetComponent<SceneNode>( _entityID );
            if( transform != nullptr && sceneNode != nullptr )
            {
                UpdateLocalTransform( followTransform , *transform, *sceneNode );
            }
        }
        ImGui::SameLine();
        ImGui::Text( "lock transform " );
        ImGui::SameLine();
        ImGui::FanShowHelpMarker( "press L to toggle" );

        // draw the local transform offset
        /*if( followTransform.mTargetTransform != nullptr && _world.HasComponent<Transform>( _entityID ) )
        {
            btTransform& target = followTransform.mTargetTransform->mTransform;
            btTransform& local = followTransform.mLocalTransform;
            btTransform t = target * local;

            _world.GetSingleton<RenderDebug>().DebugLine( target.getOrigin(), t.getOrigin(), Color::sRed );
        }*/
    }
}