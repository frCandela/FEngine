#include <editor/singletons/fanEditorPlayState.hpp>
#include "core/ecs/fanEcsSystem.hpp"

namespace fan
{

    //========================================================================================================
    // Draw the bounds of all scene nodes
    //========================================================================================================
    struct STestSystem : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<FxRigidbody>()
                   | _world.GetSignature<FxSphereCollider>()
                   | _world.GetSignature<FxTransform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, Fixed _delta )
        {
            FxPhysicsWorld& physicsWorld = _world.GetSingleton<FxPhysicsWorld>();
            if( _delta != 0 )
            {
                auto rbIt0        = _view.begin<FxRigidbody>();
                auto sphereIt0    = _view.begin<FxSphereCollider>();
                auto transformIt0 = _view.begin<FxTransform>();
                for( ; rbIt0 != _view.end<FxRigidbody>(); ++rbIt0, ++transformIt0, ++sphereIt0 )
                {
                    FxRigidbody     & rb0        = *rbIt0;
                    FxSphereCollider& sphere0    = *sphereIt0;
                    FxTransform     & transform0 = *transformIt0;
                    (void)rb0;
                    (void)sphere0;
                    (void)transform0;
                    (void)physicsWorld;
                }
            }
        }
    };
}