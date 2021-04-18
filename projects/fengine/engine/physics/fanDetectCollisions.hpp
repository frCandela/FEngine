#include "core/ecs/fanEcsSystem.hpp"
#include "engine/physics/fanFxPhysicsWorld.hpp"
#include "engine/physics/fanFxRigidbody.hpp"
#include "engine/physics/fanFxBoxCollider.hpp"
#include "engine/physics/fanFxSphereCollider.hpp"
#include "engine/components/fanFxTransform.hpp"

namespace fan
{

    //========================================================================================================
    //========================================================================================================
    struct SDetectCollisions : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<FxRigidbody>() | _world.GetSignature<FxTransform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, Fixed _delta )
        {
            RenderDebug   & rd           = _world.GetSingleton<RenderDebug>();
            FxPhysicsWorld& physicsWorld = _world.GetSingleton<FxPhysicsWorld>(); (void)_delta;
            if( _delta != 0 )
            {
                physicsWorld.mCollisionDetection.mContacts.clear();

                struct RigidbodyData
                {
                    FxTransform     * transform;
                    FxRigidbody     * rigidbody;
                    FxSphereCollider* sphere;
                    FxBoxCollider   * box;
                };

                std::vector<RigidbodyData> bodies;
                auto                       rbIt        = _view.begin<FxRigidbody>();
                auto                       transformIt = _view.begin<FxTransform>();
                for( ; rbIt != _view.end<FxRigidbody>(); ++rbIt, ++transformIt )
                {
                    EcsEntity     entity   = rbIt.GetEntity();
                    RigidbodyData bodyData = { &( *transformIt ),
                                               &( *rbIt ),
                                               _world.HasComponent<FxSphereCollider>( entity ) ? &_world.GetComponent<FxSphereCollider>( entity ) : nullptr,
                                               _world.HasComponent<FxBoxCollider>( entity ) ? &_world.GetComponent<FxBoxCollider>( entity ) : nullptr };
                    if( bodyData.box != nullptr || bodyData.sphere != nullptr ){ bodies.push_back( bodyData ); }
                }

                for( int i = 0; i < bodies.size(); i++ )
                {
                    RigidbodyData& rb0 = bodies[i];
                    if( rb0.sphere ){ CollisionDetection::SphereWithPlane( *rb0.rigidbody, *rb0.sphere, *rb0.transform, Vector3::sUp, 0, physicsWorld ); }
                    if( rb0.box ){ CollisionDetection::BoxWithPlane( *rb0.rigidbody, *rb0.box, *rb0.transform, Vector3::sUp, 0, physicsWorld ); }

                    for( int j = i + 1; j < bodies.size(); j++ )
                    {
                        RigidbodyData& rb1 = bodies[j];
                        if( rb0.sphere )
                        {
                            if( rb1.sphere )
                            {
                                CollisionDetection::SphereWithSphere( *rb0.rigidbody, *rb0.sphere, *rb0.transform, *rb1.rigidbody, *rb1.sphere, *rb1.transform, physicsWorld );
                            }
                            if( rb1.box )
                            {
                                CollisionDetection::SphereWithBox( *rb0.rigidbody, *rb0.sphere, *rb0.transform, *rb1.rigidbody, *rb1.box, *rb1.transform, physicsWorld );
                            }
                        }
                        if( rb0.box )
                        {
                            if( rb1.sphere )
                            {
                                CollisionDetection::SphereWithBox( *rb1.rigidbody, *rb1.sphere, *rb1.transform, *rb0.rigidbody, *rb0.box, *rb0.transform, physicsWorld );
                            }
                            if( rb1.box )
                            {
                                CollisionDetection::BoxWithBox( *rb0.rigidbody, *rb0.box, *rb1.rigidbody, *rb1.box, physicsWorld );
                            }
                        }
                    }
                }
                physicsWorld.mContactSolver.ResolveContacts( physicsWorld.mCollisionDetection.mContacts, _delta );
            }

            for( Contact& contact : physicsWorld.mCollisionDetection.mContacts )
            {
                rd.DebugPoint( contact.position, Color::sCyan );
            }
        }
    };
}