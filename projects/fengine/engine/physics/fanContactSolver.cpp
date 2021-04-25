#include <core/math/fanMathUtils.hpp>
#include <core/fanDebug.hpp>
#include "engine/physics/fanContactSolver.hpp"

#include "engine/physics/fanFxRigidbody.hpp"
#include "engine/components/fanFxTransform.hpp"
#include "engine/singletons/fanRenderDebug.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ContactSolver::ResolveContacts( std::vector<Contact> _contacts, const Fixed _deltaTime )
    {
        if( _contacts.empty() ){ return; }

        PrepareContacts( _contacts, _deltaTime );
        ResolvePositions( _contacts );
        ResolveVelocities( _contacts, _deltaTime );
    }

    //==================================================================================================================================================================================================
    //==============================================================================f====================================================================================================================
    void ContactSolver::PrepareContacts( std::vector<Contact>& _contacts, const Fixed _deltaTime )
    {
        for( Contact& contact : _contacts )
        {
            contact.restitution = mRestitution;
            contact.relativeContactPosition[0] = contact.position - contact.rigidbody[0]->mTransform->mPosition;
            if( contact.rigidbody[1] )
            {
                contact.relativeContactPosition[1] = contact.position - contact.rigidbody[1]->mTransform->mPosition;
            }

            // contact basis
            Vector3 tangent1, tangent2;
            Vector3::MakeOrthonormalBasis( contact.normal, tangent1, tangent2 );
            contact.contactToWorld = Matrix3( contact.normal, tangent1, tangent2 );

            // inverse mass
            contact.totalInverseMass = contact.rigidbody[0]->mInverseMass;
            if( contact.rigidbody[1] != nullptr ){ contact.totalInverseMass += contact.rigidbody[1]->mInverseMass; }

            contact.relativeVelocity          = CalculateRelativeVelocity( contact );
            contact.desiredTotalDeltaVelocity = CalculateDesiredTotalDeltaVelocity( contact, _deltaTime, mRestingVelocityLimit );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ContactSolver::ResolvePositions( std::vector<Contact>& _contacts )
    {
        for( mPositionIterationsUsed = 0; mPositionIterationsUsed < mMaxPositionsIterations; mPositionIterationsUsed++ )
        {
            Contact* worstContact = nullptr;
            Fixed worstPenetration = 0;
            for( Contact& contact : _contacts )
            {
                if( contact.penetration > worstPenetration && contact.totalInverseMass != 0 )
                {
                    worstContact     = &contact;
                    worstPenetration = contact.penetration;
                }
            }

            if( !worstContact ){ break; }

            Vector3 rotationChange[2], velocityChange[2];
            ResolvePosition( *worstContact, mAngularLimitNonLinearProjection, rotationChange, velocityChange );

            // update penetration for all contacts
            for( Contact& contact : _contacts )
            {
                if( contact.rigidbody[0] == worstContact->rigidbody[0] )
                {
                    const Vector3 deltaPosition    = velocityChange[0];
                    const Vector3 deltaRotation    = Vector3::Cross( rotationChange[0], contact.relativeContactPosition[0] );
                    const Fixed   deltaPenetration = Vector3::Dot( deltaRotation + deltaPosition, contact.normal );
                    contact.penetration -= deltaPenetration;

                    tmpRd->DebugLine( contact.position, contact.position + deltaRotation, Color::sBlue );
                    tmpRd->DebugLine( contact.position + deltaRotation, contact.position + deltaRotation + deltaPosition, Color::sCyan );
                    tmpRd->DebugLine( contact.position, contact.position + deltaRotation + deltaPosition, Color::sPurple );
                }
                else if( contact.rigidbody[0] == worstContact->rigidbody[1] )
                {
                    const Vector3 deltaPosition    = velocityChange[1];
                    const Vector3 deltaRotation    = Vector3::Cross( rotationChange[1], contact.relativeContactPosition[0] );
                    const Fixed   deltaPenetration = Vector3::Dot( deltaRotation + deltaPosition, contact.normal );
                    contact.penetration -= deltaPenetration;

                    tmpRd->DebugLine( contact.position, contact.position + deltaRotation, Color::sBlue );
                    tmpRd->DebugLine( contact.position + deltaRotation, contact.position + deltaRotation + deltaPosition, Color::sCyan );
                    tmpRd->DebugLine( contact.position, contact.position + deltaRotation + deltaPosition, Color::sPurple );
                }
                if( contact.rigidbody[1] )
                {
                    if( contact.rigidbody[1] == worstContact->rigidbody[0] )
                    {
                        const Vector3 deltaPosition    = velocityChange[0];
                        const Vector3 deltaRotation    = Vector3::Cross( rotationChange[0], contact.relativeContactPosition[1] );
                        const Fixed   deltaPenetration = Vector3::Dot( deltaRotation + deltaPosition, contact.normal );
                        contact.penetration += deltaPenetration;

                        tmpRd->DebugLine( contact.position, contact.position + deltaRotation, Color::sBlue );
                        tmpRd->DebugLine( contact.position + deltaRotation, contact.position + deltaRotation + deltaPosition, Color::sCyan );
                        tmpRd->DebugLine( contact.position, contact.position + deltaRotation + deltaPosition, Color::sPurple );
                    }
                    else if( contact.rigidbody[1] == worstContact->rigidbody[1] )
                    {
                        const Vector3 deltaPosition    = velocityChange[1];
                        const Vector3 deltaRotation    = Vector3::Cross( rotationChange[1], contact.relativeContactPosition[1] );
                        const Fixed   deltaPenetration = Vector3::Dot( deltaRotation + deltaPosition, contact.normal );
                        contact.penetration += deltaPenetration;

                        tmpRd->DebugLine( contact.position, contact.position + deltaRotation, Color::sBlue );
                        tmpRd->DebugLine( contact.position + deltaRotation, contact.position + deltaRotation + deltaPosition, Color::sCyan );
                        tmpRd->DebugLine( contact.position, contact.position + deltaRotation + deltaPosition, Color::sPurple );
                    }
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ContactSolver::ResolveVelocities( std::vector<Contact>& _contacts, const Fixed _deltaTime )
    {
        for( mVelocityIterationsUsed = 0; mVelocityIterationsUsed < mMaxVelocityIterations; mVelocityIterationsUsed++ )
        {
            Contact* worstContact = nullptr;
            Fixed maxSeparatingVelocity = 0;
            for( Contact& contact : _contacts )
            {
                const Fixed separatingVelocity = contact.relativeVelocity.x;
                if( separatingVelocity < 0 && separatingVelocity < maxSeparatingVelocity && contact.totalInverseMass != 0 )
                {
                    maxSeparatingVelocity = separatingVelocity;
                    worstContact          = &contact;
                }
            }
            if( worstContact == nullptr ){ break; }

            ResolveVelocity( *worstContact, _deltaTime );

            // updates contacts
            for( Contact& contact : _contacts )
            {
                if( worstContact->rigidbody[0] == contact.rigidbody[0] || worstContact->rigidbody[1] == contact.rigidbody[0] ||
                    ( contact.rigidbody[1] && ( worstContact->rigidbody[0] == contact.rigidbody[1] || worstContact->rigidbody[1] == contact.rigidbody[1] ) ) )
                {
                    contact.relativeVelocity          = CalculateRelativeVelocity( contact );
                    contact.desiredTotalDeltaVelocity = CalculateDesiredTotalDeltaVelocity( contact, _deltaTime, mRestingVelocityLimit );
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    // non linear projection
    //==================================================================================================================================================================================================
    void ContactSolver::ResolvePosition( const Contact& _contact, const Fixed _angularLimitNonLinearProjection, Vector3* _outRotationChange, Vector3* _outVelocityChange )
    {
        Fixed    angularInertia[2];
        Fixed    linearInertia[2];
        Fixed    totalInertia = 0;
        for( int i            = 0; i < 2; i++ )
        {
            // calculates the angular/linear inertia of each objects to determine the ratio of rotation/translation used for solving interpenetration
            if( _contact.rigidbody[i] )
            {
                const FxRigidbody& rb = *_contact.rigidbody[i];
                const Vector3 torquePerUnitImpulse   = Vector3::Cross( _contact.relativeContactPosition[i], _contact.normal );
                const Vector3 rotationPerUnitImpulse = rb.mInverseInertiaTensorWorld * torquePerUnitImpulse;
                const Vector3 velocityPerUnitImpulse = Vector3::Cross( rotationPerUnitImpulse, _contact.relativeContactPosition[i] );
                angularInertia[i] = Vector3::Dot( velocityPerUnitImpulse, _contact.normal );
                linearInertia[i]  = rb.mInverseMass;
                totalInertia += linearInertia[i] + angularInertia[i];
            }
        }

        // calculate the lenght of rotation/translation needed
        const Fixed inverseTotalInertia = 1 / totalInertia;
        Fixed       linearMove[2]       = {
                _contact.penetration * linearInertia[0] * inverseTotalInertia,
                -_contact.penetration * linearInertia[1] * inverseTotalInertia
        };
        Fixed       angularMove[2]      = {
                _contact.penetration * angularInertia[0] * inverseTotalInertia,
                -_contact.penetration * angularInertia[1] * inverseTotalInertia
        };

        // clamp the maximum rotation possible depending on the size of the object
        for( int i = 0; i < 2; i++ )
        {
            const Fixed objectSize   = _contact.relativeContactPosition->Magnitude();
            const Fixed angularLimit = _angularLimitNonLinearProjection * objectSize;
            if( Fixed::Abs( angularMove[i] ) > angularLimit )
            {
                Fixed total = angularMove[i] + linearMove[i];
                angularMove[i] = angularMove[i] > 0 ? angularLimit : -angularLimit;
                linearMove[i]  = total - angularMove[i];
            }
        }

        for( int i = 0; i < 2; i++ )
        {
            if( _contact.rigidbody[i] )
            {
                FxRigidbody& rb = *_contact.rigidbody[i];
                const Vector3 velocityChange = _contact.normal * linearMove[i];
                rb.mTransform->mPosition += velocityChange; // translate to solve interpenetration
                _outVelocityChange[i] = velocityChange;

                if( angularInertia[i] != 0 )
                {
                    // get the amount of rotation for a unit move
                    const Vector3 torquePerMove   = Vector3::Cross( _contact.relativeContactPosition[i], _contact.normal );
                    const Vector3 impulsePerMove  = rb.mInverseInertiaTensorWorld * torquePerMove;
                    Vector3       rotationPerMove = impulsePerMove / angularInertia[i];
                    Vector3       rotationChange  = angularMove[i] * rotationPerMove;

                    // rotate to solve interpenetration
                    Fixed magnitude = rotationChange.Magnitude();
                    if( !Fixed::IsFuzzyZero( magnitude ) )
                    {
                        _outRotationChange[i] = rotationChange;
                        rb.mTransform->mRotation = rb.mTransform->mRotation + FIXED( 0.5 ) * Quaternion( 0, rotationChange ) * rb.mTransform->mRotation;
                        rb.mTransform->mRotation.Normalize();
                    }
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ContactSolver::ResolveVelocity( const Contact& _contact, const Fixed _deltaTime )
    {
        tmpRd->DebugPoint( _contact.position, Color::sRed );

        // build a matrix to convert contact impulse into velocity change
        const Matrix3 impulseToTorque        = Matrix3::SkewSymmetric( _contact.relativeContactPosition[0] );
        const Matrix3 torquePerUnitImpulse   = impulseToTorque;
        const Matrix3 rotationPerUnitImpulse = _contact.rigidbody[0]->mInverseInertiaTensorWorld * torquePerUnitImpulse;
        Matrix3       velocityPerUnitImpulse = -rotationPerUnitImpulse * impulseToTorque;

        if( _contact.rigidbody[1] )
        {
            const Matrix3 impulseToTorque1        = Matrix3::SkewSymmetric( _contact.relativeContactPosition[1] );
            const Matrix3 torquePerUnitImpulse1   = impulseToTorque1;
            const Matrix3 rotationPerUnitImpulse1 = _contact.rigidbody[1]->mInverseInertiaTensorWorld * torquePerUnitImpulse1;
            Matrix3       velocityPerUnitImpulse1 = -rotationPerUnitImpulse1 * impulseToTorque1;

            velocityPerUnitImpulse += velocityPerUnitImpulse1;
        }

        // add linear velocity change
        velocityPerUnitImpulse.e11 += _contact.totalInverseMass;
        velocityPerUnitImpulse.e22 += _contact.totalInverseMass;
        velocityPerUnitImpulse.e33 += _contact.totalInverseMass;

        const Matrix3 velocityPerUnitImpulseContact = _contact.contactToWorld.Transpose() * velocityPerUnitImpulse * _contact.contactToWorld;
        const Matrix3 impulsePerUnitVelocityContact = velocityPerUnitImpulseContact.Inverse();

        // const Fixed planarImpulse0 = Fixed::Sqrt( impulseContact0.y * impulseContact0.y + impulseContact0.z * impulseContact0.z );

        // apply impulse to first body
        if( _contact.rigidbody[0]->mInverseMass != 0 )
        {
            const Fixed   deltaVelocityPerUnitImpulse0 = ContactSolver::CalculateDeltaVelocityPerUnitImpulse( _contact, 0 );
            const Fixed   desiredDeltaVelocity0        = _contact.rigidbody[0]->mInverseMass * _contact.desiredTotalDeltaVelocity / _contact.totalInverseMass;
            const Vector3 impulseFriction0             = impulsePerUnitVelocityContact * Vector3( 0, -_contact.relativeVelocity.y, -_contact.relativeVelocity.z );
            const Vector3 impulseContact               = Vector3( desiredDeltaVelocity0 / deltaVelocityPerUnitImpulse0, 0, 0 ) + impulseFriction0;
            const Vector3 impulse                      = _contact.contactToWorld * impulseContact;
            const Vector3 velocityChange               = impulse * _contact.rigidbody[0]->mInverseMass;
            const Vector3 impulsiveTorque              = Vector3::Cross( _contact.relativeContactPosition[0], impulse );
            const Vector3 rotationChange               = _contact.rigidbody[0]->mInverseInertiaTensorWorld * impulsiveTorque;
            if( _deltaTime != 0 )
            {
                _contact.rigidbody[0]->mVelocity += velocityChange;
                _contact.rigidbody[0]->mRotation += rotationChange;
            }
        }

        // apply impulse to second body
        if( _contact.rigidbody[1] && _contact.rigidbody[1]->mInverseMass != 0 )
        {
            const Fixed   deltaVelocityPerUnitImpulse1 = ContactSolver::CalculateDeltaVelocityPerUnitImpulse( _contact, 1 );
            const Fixed   desiredDeltaVelocity1        = _contact.rigidbody[1]->mInverseMass * _contact.desiredTotalDeltaVelocity / _contact.totalInverseMass;
            const Vector3 impulseFriction1             = impulsePerUnitVelocityContact * Vector3( 0, -_contact.relativeVelocity.y, -_contact.relativeVelocity.z );
            const Vector3 impulseContact1              = Vector3( desiredDeltaVelocity1 / deltaVelocityPerUnitImpulse1, 0, 0 ) + impulseFriction1;
            const Vector3 impulse1                     = _contact.contactToWorld * impulseContact1;
            const Vector3 velocityChange1              = impulse1 * _contact.rigidbody[1]->mInverseMass;
            const Vector3 impulsiveTorque1             = Vector3::Cross( _contact.relativeContactPosition[1], impulse1 );
            const Vector3 rotationChange1              = _contact.rigidbody[1]->mInverseInertiaTensorWorld * impulsiveTorque1;
            if( _deltaTime != 0 )
            {
                _contact.rigidbody[1]->mVelocity -= velocityChange1;
                _contact.rigidbody[1]->mRotation -= rotationChange1;
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Fixed ContactSolver::CalculateDeltaVelocityPerUnitImpulse( const Contact& _contact, const int _index )
    {
        Vector3 torquePerUnitImpulse          = Vector3::Cross( _contact.relativeContactPosition[_index], _contact.normal );
        Vector3 rotationPerUnitImpulse        = _contact.rigidbody[_index]->mInverseInertiaTensorWorld * torquePerUnitImpulse;
        Vector3 velocityPerUnitImpulse        = Vector3::Cross( rotationPerUnitImpulse, _contact.relativeContactPosition[_index] );
        Vector3 velocityPerUnitImpulseContact = _contact.contactToWorld.Transpose() * velocityPerUnitImpulse;
        Fixed   deltaVelocityPerUnitImpulse   = velocityPerUnitImpulseContact.x; // > 0
        deltaVelocityPerUnitImpulse += _contact.rigidbody[_index]->mInverseMass;  // linear component
        return deltaVelocityPerUnitImpulse;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Vector3 ContactSolver::CalculateRelativeVelocity( const Contact& _contact )
    {
        Vector3 relativeVelocity = Vector3::Cross( _contact.rigidbody[0]->mRotation, _contact.relativeContactPosition[0] );
        relativeVelocity += _contact.rigidbody[0]->mVelocity;
        if( _contact.rigidbody[1] )
        {
            relativeVelocity -= Vector3::Cross( _contact.rigidbody[1]->mRotation, _contact.relativeContactPosition[1] );
            relativeVelocity -= _contact.rigidbody[1]->mVelocity;
        }
        relativeVelocity         = _contact.contactToWorld.Transpose() * relativeVelocity;
        return relativeVelocity;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Fixed ContactSolver::CalculateDesiredTotalDeltaVelocity( const Contact& _contact, const Fixed _deltaTime, Fixed _restingVelocityLimit )
    {
        // Velocity from constant acceleration is removed to prevent vibration
        Fixed velocityFromAcceleration = Vector3::Dot( _contact.rigidbody[0]->mAcceleration, _contact.normal );
        if( _contact.rigidbody[1] != nullptr )
        {
            velocityFromAcceleration -= Vector3::Dot( _contact.rigidbody[1]->mAcceleration, _contact.normal );
        }
        velocityFromAcceleration *= _deltaTime;

        // restitution is set to zero below a threshold to prevent vibration
        const Fixed restitution = Fixed::Abs( _contact.relativeVelocity.x ) < _restingVelocityLimit ? 0 : _contact.restitution;

        return ( -_contact.relativeVelocity.x - restitution * ( _contact.relativeVelocity.x - velocityFromAcceleration ) );
    }
}