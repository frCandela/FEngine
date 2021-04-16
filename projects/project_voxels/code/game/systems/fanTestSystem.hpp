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
               // EditorPlayState& ps = _world.GetSingleton<EditorPlayState>();
                //if( ps.mState == EditorPlayState::PLAYING ){ ps.mState = EditorPlayState::PAUSED; }

                // calculate the velocity delta per unit contact impulse
                Vector3 torquePerUnitImpulse = Vector3::Cross( _contact.relativeContactPosition0, _contact.normal );
                Vector3 rotationPerUnitImpulse = _contact.rb0->mInverseInertiaTensorWorld * torquePerUnitImpulse;
                Vector3 velocityPerUnitImpulse = Vector3::Cross( rotationPerUnitImpulse, _contact.relativeContactPosition0 );
                Vector3 velocityPerUnitImpulseContact = _contact.contactToWorld.Transpose() * velocityPerUnitImpulse;
                Fixed deltaVelocityPerUnitImpulse = velocityPerUnitImpulseContact.x; // > 0
                deltaVelocityPerUnitImpulse += _contact.rb0->mInverseMass;  // linear component

                // calculates the required impulse
                Fixed separatingVelocity = ContactSolver::CalculateSeparatingVelocity(_contact);
                Fixed desiredDeltaVelocity = -separatingVelocity * ( 1 + _contact.restitution);
                Vector3 impulseContact( desiredDeltaVelocity / deltaVelocityPerUnitImpulse, 0, 0 );

                Vector3 impulse = _contact.contactToWorld * impulseContact;
                Vector3 velocityChange = impulse * _contact.rb0->mInverseMass;
                Vector3 impulsiveTorque = Vector3::Cross( _contact.relativeContactPosition0, impulse );
                Vector3 rotationChange = _contact.rb0->mInverseInertiaTensorWorld * impulsiveTorque;

                Vector3 aaa = Vector3::Cross( _contact.rb0->mRotation, _contact.relativeContactPosition0 );
                rd.DebugLine( _contact.position, _contact.position + aaa, Color::sBlue );
                rd.DebugLine( _contact.position, _contact.position + rotationChange, Color::sCyan );

                rd.DebugLine( _contact.transform0->mPosition, _contact.transform0->mPosition + _contact.relativeContactPosition0, Color::sGreen );
                rd.DebugLine( _contact.position, _contact.position + _contact.normal, Color::sOrange );
                rd.DebugPoint( _contact.position, Color::sYellow );
            }
        }
    };
}