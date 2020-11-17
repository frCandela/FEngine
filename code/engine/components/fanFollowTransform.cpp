#include "engine/components/fanFollowTransform.hpp"
#include "engine/components/fanTransform.hpp"
#include "fanSceneNode.hpp"
#include "engine/systems/fanUpdateTransforms.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/fanSceneSerializable.hpp"
#include "engine/singletons/fanScene.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void FollowTransform::SetInfo( EcsComponentInfo& _info )
	{
		_info.save        = &FollowTransform::Save;
		_info.load        = &FollowTransform::Load;
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
		Serializable::SaveBool( _json, "locked", followTransform.mLocked );
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
		Serializable::LoadBool( _json, "locked", followTransform.mLocked );
	}
}