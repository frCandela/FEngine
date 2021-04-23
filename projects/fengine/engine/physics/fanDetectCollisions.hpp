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
            //if( _delta != 0 )
            {
                struct RigidbodyData
                {
                    FxTransform     * transform;
                    FxRigidbody     * rigidbody;
                    FxSphereCollider* sphere;
                    FxBoxCollider   * box;
                };

                // Get all bodies
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
                                CollisionDetection::BoxWithBox( *rb0.rigidbody, *rb0.box, *rb1.rigidbody, *rb1.box, contacts );
                            }
                        }
                    }
                }
                physicsWorld.mContactSolver.tmpRd = &rd;
                physicsWorld.mContactSolver.ResolveContacts( physicsWorld.mCollisionDetection.mContacts );
            }

            /*for( Contact& _contact : physicsWorld.mCollisionDetection.mContacts )
            {
               // if( _delta > 0)                _world.GetSingleton<EditorPlayState>().mState = EditorPlayState::PAUSED;

                //rd.DebugPoint( _contact.position, Color::sCyan );
               // rd.DebugLine( _contact.position, _contact.position + _contact.penetration * _contact.normal, Color::sCyan );

                Fixed       angularInertia[2];
                Fixed       linearInertia[2];
                Fixed       totalInertia = 0;
                for( int    i                   = 0; i < 2; i++ )
                {
                    if( _contact.rigidbody[i] )
                    {
                        const FxRigidbody& rb = *_contact.rigidbody[i];
                        const Vector3 torquePerUnitImpulse   = Vector3::Cross( _contact.relativeContactPosition[i], _contact.normal );
                        const Vector3 rotationPerUnitImpulse = rb.mInverseInertiaTensorWorld * torquePerUnitImpulse;
                        const Vector3 velocityPerUnitImpulse = Vector3::Cross( rotationPerUnitImpulse, _contact.relativeContactPosition[i]);
                        angularInertia[i] = Vector3::Dot( velocityPerUnitImpulse, _contact.normal );
                        linearInertia[i]  = rb.mInverseMass;
                        totalInertia += linearInertia[i] + angularInertia[i];
                    }
                }

                const Fixed inverseTotalInertia = 1 / totalInertia;
                Fixed       linearMove[2]       = {
                        _contact.penetration * linearInertia[0] * inverseTotalInertia,
                        -_contact.penetration * linearInertia[1] * inverseTotalInertia
                };
                Fixed       angularMove[2]      = {
                        _contact.penetration * angularInertia[0] * inverseTotalInertia,
                        -_contact.penetration * angularInertia[1] * inverseTotalInertia
                };

                for( int i = 0; i < 2; i++ )
                {
                    // clamp the maximum rotation
                    Fixed angularLimit = physicsWorld.mContactSolver.mAngularLimitNonLinearProjection * _contact.relativeContactPosition->Magnitude();
                    if( Fixed::Abs( angularMove[i] ) > angularLimit )
                    {
                        Fixed total = angularMove[i] + linearMove[i];
                        angularMove[i] = angularMove[i] > 0 ? angularLimit : -angularLimit;
                        linearMove[i]  = total - angularMove[i];
                    }
                }

                for( int    i                   = 0; i < 2; i++ )
                {
                    if( _contact.rigidbody[i] )
                    {
                        const  FxRigidbody& rb = *_contact.rigidbody[i];
                        //rb.mTransform->mPosition += _contact.normal * linearMove[i];

                        // get the amount of rotation for a unit move
                        if( angularInertia[i] != 0)
                        {
                            const Vector3 torquePerMove   = Vector3::Cross( _contact.relativeContactPosition[i], _contact.normal );
                            const Vector3 impulsePerMove  = rb.mInverseInertiaTensorWorld * torquePerMove;
                            Vector3       rotationPerMove = impulsePerMove / angularInertia[i];
                            Vector3       rotation        = angularMove[i] * rotationPerMove;

                            const Vector3 aMove = Vector3::Cross( rotation, _contact.relativeContactPosition[i]);
                            rd.DebugLine( _contact.position, _contact.position +  aMove, Color::sRed );
                            const Vector3 lMove = _contact.normal * linearMove[i];
                            rd.DebugLine( _contact.position + aMove, _contact.position + aMove + lMove, Color::sOrange );

                            Fixed magnitude = rotation.Magnitude();
                            if( !Fixed::IsFuzzyZero( magnitude ) )
                            {
                                // Quaternion rotQ = Quaternion::AngleAxisRadians( magnitude, rotation / magnitude );s
                                Vector3 origin = rb.mTransform->mRotation.Inverse() * _contact.relativeContactPosition[i];
                                Vector3 oldRotation =  rb.mTransform->mRotation *  origin;

                               Quaternion newRot =  rb.mTransform->mRotation + FIXED( 0.5 ) * Quaternion( 0, rotation ) *  rb.mTransform->mRotation;
                                newRot.Normalize();

                                Vector3 newRotation =  newRot *  origin;

                                rd.DebugLine( rb.mTransform->mPosition, rb.mTransform->mPosition + oldRotation, Color::sBlue );
                                rd.DebugLine( rb.mTransform->mPosition, rb.mTransform->mPosition + newRotation, Color::sCyan );
                            }
                        }
                    }
                }
            }*/
        }
    };
}