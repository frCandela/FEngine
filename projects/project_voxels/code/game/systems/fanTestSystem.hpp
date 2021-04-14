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
            FxPhysicsWorld physicsWorld = _world.GetSingleton<FxPhysicsWorld>();
            physicsWorld.mCollisionDetection.mContacts.clear();

            std::unordered_set<CollisionPair> collidingPairs;

            auto     rbIt0        = _view.begin<FxRigidbody>();
            auto     sphereIt0    = _view.begin<FxSphereCollider>();
            auto     transformIt0 = _view.begin<FxTransform>();
            for( int i            = 0; rbIt0 != _view.end<FxRigidbody>(); ++rbIt0, ++transformIt0, ++sphereIt0, ++i )
            {
                FxRigidbody     & rb0        = *rbIt0;
                FxSphereCollider& sphere0    = *sphereIt0;
                FxTransform     & transform0 = *transformIt0;
                // contact with the ground
                if( transform0.mPosition.y - FIXED( 0.5 ) < 0 )
                {
                    Contact contact;
                    contact.rb0        = &rb0;
                    contact.transform0 = &transform0;
                    contact.normal     = Vector3::sUp;
                    Vector3::MakeOrthonormalBasis( contact.normal, contact.tangents[0], contact.tangents[1] );
                    contact.contactToWorld           = Matrix3( contact.normal, contact.tangents[0], contact.tangents[1] );
                    contact.position                 = Vector3( transform0.mPosition.x, transform0.mPosition.y - FIXED( 0.5 ), transform0.mPosition.z );
                    contact.relativeContactPosition0 = contact.position - contact.transform0->mPosition;
                    contact.restitution              = physicsWorld.mRestitution;
                    contact.penetration              = FIXED( 0.5 ) - transform0.mPosition.y;
                    contact.rb1                      = nullptr;
                    contact.transform1               = nullptr;
                    physicsWorld.mCollisionDetection.mContacts.push_back( contact );
                }

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
                        if( ! collidingPairs.contains({j,i}) && CollisionDetection::SphereWithSphere( rb0, sphere0, transform0, rb1, sphere1, transform1, physicsWorld ) )
                        {
                            collidingPairs.insert({i,j});
                        }
                    }
                }
            }
            physicsWorld.mContactSolver.ResolveContacts( physicsWorld.mCollisionDetection.mContacts, _delta );


            // draw debug
            RenderDebug& rd = _world.GetSingleton<RenderDebug>();
            for( Contact c : physicsWorld.mCollisionDetection.mContacts )
            {
                rd.DebugPoint( ToBullet( Math::ToGLM( c.position ) ), Color::sRed );
                rd.DebugLine( ToBullet( Math::ToGLM( c.position ) ), ToBullet( Math::ToGLM( c.position + c.normal ) ), Color::sYellow, false );
            };
        }
    };
}