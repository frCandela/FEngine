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
            FxPhysicsWorld&physicsWorld = _world.GetSingleton<FxPhysicsWorld>();
            if( _delta != 0 )
            {
                physicsWorld.mCollisionDetection.mContacts.clear();
                std::unordered_set < CollisionPair > collidingPairs;
                auto     rbIt0        = _view.begin<FxRigidbody>();
                auto     sphereIt0    = _view.begin<FxSphereCollider>();
                auto     transformIt0 = _view.begin<FxTransform>();
                for( int i            = 0; rbIt0 != _view.end<FxRigidbody>(); ++rbIt0, ++transformIt0, ++sphereIt0, ++i )
                {
                    FxRigidbody     & rb0        = *rbIt0;
                    FxSphereCollider& sphere0    = *sphereIt0;
                    FxTransform     & transform0 = *transformIt0;
                    CollisionDetection::SphereWithPlane( rb0, sphere0, transform0, Vector3::sUp, 0, physicsWorld );

                    // contact with other spheres
                    auto     rbIt1        = _view.begin<FxRigidbody>();
                    auto     sphereIt1    = _view.begin<FxSphereCollider>();
                    auto     transformIt1 = _view.begin<FxTransform>();
                    for( int j            = 0; rbIt1 != _view.end<FxRigidbody>(); ++rbIt1, ++transformIt1, ++sphereIt1, ++j )
                    {
                        FxRigidbody     & rb1        = *rbIt1;
                        FxTransform     & transform1 = *transformIt1;
                        FxSphereCollider& sphere1    = *sphereIt1;
                        if( i != j )
                        {
                            if( !collidingPairs.contains( { j, i } ) && CollisionDetection::SphereWithSphere( rb0, sphere0, transform0, rb1, sphere1, transform1, physicsWorld ) )
                            {
                                collidingPairs.insert( { i, j } );
                            }
                        }
                    }
                }
                physicsWorld.mContactSolver.ResolveContacts( physicsWorld.mCollisionDetection.mContacts, _delta );
            }

            // draw debug
            RenderDebug& rd = _world.GetSingleton<RenderDebug>();
            for( Contact c : physicsWorld.mCollisionDetection.mContacts )
            {
                rd.DebugPoint( ToBullet( Math::ToGLM( c.position ) ), Color::sRed );
                rd.DebugLine( ToBullet( Math::ToGLM( c.position ) ), ToBullet( Math::ToGLM( c.position + c.normal ) ), Color::sYellow, false );
                rd.DebugLine( ToBullet( Math::ToGLM( c.position ) ), ToBullet( Math::ToGLM( c.position + c.relativeContactPosition0 ) ), Color::sGreen, false );
            }
        }
    };
}