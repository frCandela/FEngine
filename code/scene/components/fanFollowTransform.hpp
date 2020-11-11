#pragma  once

#include "core/ecs/fanEcsComponent.hpp"
#include "scene/components/fanTransform.hpp"

namespace fan
{
    struct SceneNode;
    struct Transform;

	//========================================================================================================
	// makes a transform follow another transform
	//========================================================================================================
	struct FollowTransform : public EcsComponent
	{
		ECS_COMPONENT( FollowTransform )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		btTransform             mLocalTransform;
		bool                    mLocked;

        static void UpdateLocalTransform( FollowTransform& _followTransform,
                                          Transform& _transform,
                                          SceneNode& _sceneNode );
	};
}