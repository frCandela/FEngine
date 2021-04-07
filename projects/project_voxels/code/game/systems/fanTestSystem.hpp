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
            return _world.GetSignature<FxRigidbody>() | _world.GetSignature<FxTransform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, Fixed _delta )
        {
            FxPhysicsWorld physicsWorld = _world.GetSingleton<FxPhysicsWorld>();

            auto rbIt        = _view.begin<FxRigidbody>();
            auto transformIt = _view.begin<FxTransform>();
            for( ; rbIt != _view.end<FxRigidbody>(); ++rbIt, ++transformIt )
            {
                FxRigidbody& rb        = *rbIt;
                FxTransform& transform = *transformIt;

                //rb.ApplyForce( -physicsWorld.mGravity );

                if( transform.mPosition.y - FIXED( 0.5 ) < 0 )
                {
                    /*transform.mPosition.y = FIXED(0.5);
                    rb.mVelocity.y = 0;*/

                    Contact contact;
                    contact.rb0         = &rb;
                    contact.transform0  = &transform;
                    contact.normal      = Vector3::sUp;
                    contact.restitution = FIXED( 0.5 );
                    contact.penetration = -( transform.mPosition.y - FIXED( 0.5 ) );
                    contact.rb1         = nullptr;
                    contact.transform1 = nullptr;
                    physicsWorld.mContactSolver.ResolveContacts( {contact}, _delta );
                }

                (void)rb;
                (void)transform;
            }
        }
    };
}