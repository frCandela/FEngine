#include <editor/singletons/fanEditorPlayState.hpp>
#include "core/ecs/fanEcsSystem.hpp"

namespace fan
{

    //==================================================================================================================================================================================================
    // Draw the bounds of all scene nodes
    //==================================================================================================================================================================================================
    struct STestSystem : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Rigidbody>()
                   | _world.GetSignature<SphereCollider>()
                   | _world.GetSignature<Transform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, Fixed _delta )
        {
            PhysicsWorld& physicsWorld = _world.GetSingleton<PhysicsWorld>();
            if( _delta != 0 )
            {
                auto rbIt0        = _view.begin<Rigidbody>();
                auto sphereIt0    = _view.begin<SphereCollider>();
                auto transformIt0 = _view.begin<Transform>();
                for( ; rbIt0 != _view.end<Rigidbody>(); ++rbIt0, ++transformIt0, ++sphereIt0 )
                {
                    Rigidbody     & rb0        = *rbIt0;
                    SphereCollider& sphere0    = *sphereIt0;
                    Transform     & transform0 = *transformIt0;
                    (void)rb0;
                    (void)sphere0;
                    (void)transform0;
                    (void)physicsWorld;
                }
            }
        }
    };
}