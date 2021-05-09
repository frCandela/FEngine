#include "engine/components/fanFollowTransform.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/systems/fanUpdateTransforms.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/fanEngineSerializable.hpp"
#include "engine/singletons/fanScene.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FollowTransform::SetInfo( EcsComponentInfo& _info )
    {
        _info.save = &FollowTransform::Save;
        _info.load = &FollowTransform::Load;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FollowTransform::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        // clear
        FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
        followTransform.mLocalTransform.mRotation = Quaternion::sIdentity;
        followTransform.mLocalTransform.mPosition = Vector3::sZero;
        followTransform.mLocked         = false;
    }

    //==================================================================================================================================================================================================
    // calculates the new local transform from the follower & target transforms
    //==================================================================================================================================================================================================
    void FollowTransform::UpdateLocalTransform( FollowTransform& _followTransform, FxTransform& _transform, SceneNode& _sceneNode )
    {
        fanAssert( _sceneNode.mParentHandle != 0 );
        EcsWorld& world = *_sceneNode.mScene->mWorld;
        EcsEntity parentEntity = world.GetEntity( _sceneNode.mParentHandle );
        FxTransform* parentTransform = world.SafeGetComponent<FxTransform>( parentEntity );

        if( parentTransform != nullptr )
        {
            _followTransform.mLocalTransform = parentTransform->Inverse() * _transform;
        }
        else
        {
            _followTransform.mLocalTransform = FxTransform::sIdentity;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FollowTransform::Save( const EcsComponent& _component, Json& _json )
    {
        const FollowTransform& followTransform = static_cast<const FollowTransform&>( _component );
        Serializable::SaveVec3( _json, "local_position", followTransform.mLocalTransform.mPosition );
        Serializable::SaveQuat( _json, "local_rotation", followTransform.mLocalTransform.mRotation );
        Serializable::SaveBool( _json, "locked", followTransform.mLocked );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FollowTransform::Load( EcsComponent& _component, const Json& _json )
    {
        FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
        Serializable::LoadVec3( _json, "local_position", followTransform.mLocalTransform.mPosition );
        Serializable::LoadQuat( _json, "local_rotation", followTransform.mLocalTransform.mRotation );
        Serializable::LoadBool( _json, "locked", followTransform.mLocked );
    }
}