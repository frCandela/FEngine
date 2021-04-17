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
            for( Contact _contact : physicsWorld.mCollisionDetection.mContacts )
            {
                EditorPlayState& ps = _world.GetSingleton<EditorPlayState>();(void)ps;
                //if( ps.mState == EditorPlayState::PLAYING ){ ps.mState = EditorPlayState::PAUSED; }

                Fixed totalInverseMass = _contact.rigidbody[0]->mInverseMass;
                if( _contact.rigidbody[1] != nullptr ){ totalInverseMass += _contact.rigidbody[1]->mInverseMass; }
                Fixed separatingVelocity = ContactSolver::CalculateSeparatingVelocity(_contact);
                Fixed desiredTotalDeltaVelocity = -separatingVelocity * ( 1 + _contact.restitution) / totalInverseMass;


                Fixed deltaVelocityPerUnitImpulse0 = ContactSolver::CalculateDeltaVelocityPerUnitImpulse(_contact, 0);
                Fixed desiredDeltaVelocity0 = _contact.rigidbody[0]->mInverseMass * desiredTotalDeltaVelocity;
                Vector3 impulseContact( desiredDeltaVelocity0 / deltaVelocityPerUnitImpulse0, 0, 0 );
                Vector3 impulse = _contact.contactToWorld * impulseContact;
                Vector3 velocityChange = impulse * _contact.rigidbody[0]->mInverseMass;
                Vector3 impulsiveTorque = Vector3::Cross( _contact.relativeContactPosition[0], impulse );
                Vector3 rotationChange = _contact.rigidbody[0]->mInverseInertiaTensorWorld * impulsiveTorque;



                if (_contact.rigidbody[1])
                {
                    Fixed desiredDeltaVelocity1 = -_contact.rigidbody[1]->mInverseMass * desiredTotalDeltaVelocity;
                    Fixed deltaVelocityPerUnitImpulse1 = ContactSolver::CalculateDeltaVelocityPerUnitImpulse(_contact, 1);
                    Vector3 impulseContact1( desiredDeltaVelocity1 / deltaVelocityPerUnitImpulse1, 0, 0 );
                    Vector3 impulse1 = _contact.contactToWorld * impulseContact1;
                    Vector3 velocityChange1 = impulse1 * _contact.rigidbody[1]->mInverseMass;
                    Vector3 impulsiveTorque1 = Vector3::Cross( _contact.relativeContactPosition[1], impulse1 );
                    Vector3 rotationChange1 = _contact.rigidbody[1]->mInverseInertiaTensorWorld * impulsiveTorque1;

                    (void)rotationChange1;
                    rd.DebugLine( _contact.position, _contact.position + _contact.rigidbody[1]->mVelocity, Color::sCyan );
                    rd.DebugLine( _contact.position, _contact.position + velocityChange1, Color::sBlue );

                }


                //rd.DebugLine( _contact.position, _contact.position + _contact.normal, Color::sOrange );
                //rd.DebugPoint( _contact.position, Color::sYellow );
            }
        }
    };
}