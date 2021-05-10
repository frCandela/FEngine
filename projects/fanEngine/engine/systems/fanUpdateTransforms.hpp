#include "core/ecs/fanEcsWorld.hpp"
#include "core/ecs/fanEcsSystem.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/components/fanFollowTransform.hpp"
#include "engine/components/fanSceneNode.hpp"

namespace fan
{

    //==================================================================================================================================================================================================
    // setups the FollowTransforms offsets @todo remove this ( useless ? )
    //==================================================================================================================================================================================================
    struct SInitFollowTransforms : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Transform>() |
                   _world.GetSignature<SceneNode>() |
                   _world.GetSignature<FollowTransform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto sceneNodeIt       = _view.begin<SceneNode>();
            auto transformIt       = _view.begin<Transform>();
            auto followTransformIt = _view.begin<FollowTransform>();
            for( ; transformIt != _view.end<Transform>(); ++transformIt, ++followTransformIt, ++sceneNodeIt )
            {
                const Transform& follow          = *transformIt;
                FollowTransform& followTransform = *followTransformIt;
                SceneNode      & sceneNode       = *sceneNodeIt;

                fanAssert( sceneNode.mParentHandle != 0 );
                EcsEntity parentEntity = _world.GetEntity( sceneNode.mParentHandle );
                Transform* parentTransform = _world.SafeGetComponent<Transform>( parentEntity );
                if( parentTransform != nullptr )
                {
                    followTransform.mLocalTransform = parentTransform->Inverse() * follow;
                }
            }
        }
    };

    //==================================================================================================================================================================================================
    // moves the 3D FollowTransforms after their targets
    //==================================================================================================================================================================================================
    struct SMoveFollowTransforms : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Transform>() |
                   _world.GetSignature<SceneNode>() |
                   _world.GetSignature<FollowTransform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto sceneNodeIt       = _view.begin<SceneNode>();
            auto transformIt       = _view.begin<Transform>();
            auto followTransformIt = _view.begin<FollowTransform>();
            for( ; transformIt != _view.end<Transform>(); ++transformIt, ++followTransformIt, ++sceneNodeIt )
            {
                Transform            & follow          = *transformIt;
                const FollowTransform& followTransform = *followTransformIt;
                SceneNode            & sceneNode       = *sceneNodeIt;

                fanAssert( sceneNode.mParentHandle != 0 );
                EcsEntity parentEntity = _world.GetEntity( sceneNode.mParentHandle );
                Transform* parentTransform = _world.SafeGetComponent<Transform>( parentEntity );
                if( followTransform.mLocked && parentTransform != nullptr )
                {
                    follow = ( *parentTransform ) * followTransform.mLocalTransform;
                }
            }
        }
    };
}