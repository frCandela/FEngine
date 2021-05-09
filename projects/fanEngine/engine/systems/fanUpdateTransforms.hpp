#include "core/ecs/fanEcsWorld.hpp"
#include "core/ecs/fanEcsSystem.hpp"
#include "engine/components/fanFxTransform.hpp"
#include "engine/components/fanFollowTransform.hpp"
#include "engine/components/fanSceneNode.hpp"

namespace fan
{

    //========================================================================================================
    // setups the FollowTransforms offsets @todo remove this ( useless ? )
    //========================================================================================================
    struct SInitFollowTransforms : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<FxTransform>() |
                   _world.GetSignature<SceneNode>() |
                   _world.GetSignature<FollowTransform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto sceneNodeIt       = _view.begin<SceneNode>();
            auto transformIt       = _view.begin<FxTransform>();
            auto followTransformIt = _view.begin<FollowTransform>();
            for( ; transformIt != _view.end<FxTransform>(); ++transformIt, ++followTransformIt, ++sceneNodeIt )
            {
                const FxTransform& follow          = *transformIt;
                FollowTransform  & followTransform = *followTransformIt;
                SceneNode        & sceneNode       = *sceneNodeIt;

                fanAssert( sceneNode.mParentHandle != 0 );
                EcsEntity parentEntity = _world.GetEntity( sceneNode.mParentHandle );
                FxTransform* parentTransform = _world.SafeGetComponent<FxTransform>( parentEntity );
                if( parentTransform != nullptr )
                {
                    followTransform.mLocalTransform =  parentTransform->Inverse() * follow;
                }
            }
        }
    };

    //========================================================================================================
    // moves the 3D FollowTransforms after their targets
    //========================================================================================================
    struct SMoveFollowTransforms : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<FxTransform>() |
                   _world.GetSignature<SceneNode>() |
                   _world.GetSignature<FollowTransform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto sceneNodeIt       = _view.begin<SceneNode>();
            auto transformIt       = _view.begin<FxTransform>();
            auto followTransformIt = _view.begin<FollowTransform>();
            for( ; transformIt != _view.end<FxTransform>(); ++transformIt, ++followTransformIt, ++sceneNodeIt )
            {
                FxTransform          & follow          = *transformIt;
                const FollowTransform& followTransform = *followTransformIt;
                SceneNode            & sceneNode       = *sceneNodeIt;

                fanAssert( sceneNode.mParentHandle != 0 );
                EcsEntity parentEntity = _world.GetEntity( sceneNode.mParentHandle );
                FxTransform* parentTransform = _world.SafeGetComponent<FxTransform>( parentEntity );
                if( followTransform.mLocked && parentTransform != nullptr )
                {
                    follow = (*parentTransform) * followTransform.mLocalTransform;
                }
            }
        }
    };
}