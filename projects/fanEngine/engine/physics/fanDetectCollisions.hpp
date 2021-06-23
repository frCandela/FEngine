#include "ecs/fanEcsSystem.hpp"
#include "core/time/fanProfiler.hpp"
#include "engine/physics/fanPhysicsWorld.hpp"
#include "engine/physics/fanRigidbody.hpp"
#include "engine/physics/fanBoxCollider.hpp"
#include "engine/physics/fanSphereCollider.hpp"
#include "fanTransform.hpp"

namespace fan
{

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SDetectCollisions : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Rigidbody>() | _world.GetSignature<Transform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, Fixed _delta )
        {
            PhysicsWorld& physicsWorld = _world.GetSingleton<PhysicsWorld>();
            (void)_delta;
            //if( _delta != 0 )
            {
                SCOPED_PROFILE( detect_collisions )
                struct RigidbodyData
                {
                    Transform     * transform;
                    Rigidbody     * rigidbody;
                    SphereCollider* sphere;
                    BoxCollider   * box;
                };

                // Get all bodies
                std::vector<RigidbodyData> bodies;
                auto                       rbIt        = _view.begin<Rigidbody>();
                auto                       transformIt = _view.begin<Transform>();
                for( ; rbIt != _view.end<Rigidbody>(); ++rbIt, ++transformIt )
                {
                    EcsEntity     entity   = rbIt.GetEntity();
                    RigidbodyData bodyData = { &( *transformIt ),
                                               &( *rbIt ),
                                               _world.HasComponent<SphereCollider>( entity ) ? &_world.GetComponent<SphereCollider>( entity ) : nullptr,
                                               _world.HasComponent<BoxCollider>( entity ) ? &_world.GetComponent<BoxCollider>( entity ) : nullptr };
                    if( bodyData.box != nullptr || bodyData.sphere != nullptr )
                    {
                        bodies.push_back( bodyData );
                        bodyData.rigidbody->CalculateDerivedData( *bodyData.transform );
                    }
                }

                // Find collisions
                std::vector<Contact>& contacts = physicsWorld.mCollisionDetection.mContacts;
                contacts.clear();
                for( int i = 0; i < bodies.size(); i++ )
                {
                    RigidbodyData& rb0 = bodies[i];
                    if( rb0.sphere ){ CollisionDetection::SphereWithPlane( *rb0.rigidbody, *rb0.sphere, Vector3::sUp, 0, contacts ); }
                    if( rb0.box ){ CollisionDetection::BoxWithPlane( *rb0.rigidbody, *rb0.box, Vector3::sUp, 0, contacts ); }

                    for( int j = i + 1; j < bodies.size(); j++ )
                    {
                        RigidbodyData& rb1 = bodies[j];

                        if( rb0.rigidbody->mIsSleeping && rb1.rigidbody->mIsSleeping ){ continue; }

                        if( rb0.sphere )
                        {
                            if( rb1.sphere )
                            {
                                CollisionDetection::SphereWithSphere( *rb0.rigidbody, *rb0.sphere, *rb1.rigidbody, *rb1.sphere, contacts );
                            }
                            if( rb1.box )
                            {
                                CollisionDetection::SphereWithBox( *rb0.rigidbody, *rb0.sphere, *rb1.rigidbody, *rb1.box, contacts );
                            }
                        }
                        if( rb0.box )
                        {
                            if( rb1.sphere )
                            {
                                CollisionDetection::SphereWithBox( *rb1.rigidbody, *rb1.sphere, *rb0.rigidbody, *rb0.box, contacts );
                            }
                            if( rb1.box )
                            {
                                CollisionDetection::tmpRd = &_world.GetSingleton<RenderDebug>();;
                                CollisionDetection::BoxWithBox( *rb0.rigidbody, *rb0.box, *rb1.rigidbody, *rb1.box, contacts );
                            }
                        }
                    }
                }
                physicsWorld.mContactSolver.tmpRd = &_world.GetSingleton<RenderDebug>();;
                physicsWorld.mContactSolver.ResolveContacts( physicsWorld.mCollisionDetection.mContacts, _delta );
            }
        }
    };
}