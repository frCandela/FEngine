#include <core/math/fanMathUtils.hpp>
#include <core/fanDebug.hpp>
#include "engine/physics/fanContactSolver.hpp"

#include "engine/physics/fanFxRigidbody.hpp"
#include "engine/components/fanFxTransform.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ContactSolver::ResolveContacts( std::vector<Contact> _contacts )
    {
        while( !_contacts.empty() )
        {
            // find the contact with the largest closing velocity
            Fixed    maxSeparatingVelocity = 0;
            int      maxIndex              = (int)_contacts.size();
            for( int i                     = 0; i < (int)_contacts.size(); ++i )
            {
                const Fixed separatingVelocity = CalculateSeparatingVelocity( _contacts[i] );
                if( separatingVelocity < 0 && separatingVelocity < maxSeparatingVelocity )
                {
                    maxSeparatingVelocity = separatingVelocity;
                    maxIndex              = i;
                }
            }
            if( maxIndex == (int)_contacts.size() ){ break; }

            Contact contact = _contacts[maxIndex];
            _contacts[maxIndex] = _contacts[_contacts.size() - 1];
            _contacts.pop_back();
            ResolveVelocity( contact );
            ResolveInterpenetration( contact );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Fixed ContactSolver::CalculateSeparatingVelocity( const Contact& _contact )
    {
        Vector3 separatingVelocity = Vector3::Cross( _contact.rigidbody[0]->mRotation, _contact.relativeContactPosition[0] );
        separatingVelocity += _contact.rigidbody[0]->mVelocity;
        if( _contact.rigidbody[1] )
        {
            separatingVelocity -= Vector3::Cross( _contact.rigidbody[1]->mRotation, _contact.relativeContactPosition[1] );
            separatingVelocity -= _contact.rigidbody[1]->mVelocity;
        }
        separatingVelocity         = _contact.contactToWorld.Transpose() * separatingVelocity;
        return separatingVelocity.x;
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
    void ContactSolver::ResolveVelocity( const Contact& _contact )
    {
        Fixed totalInverseMass = _contact.rigidbody[0]->mInverseMass;
        if( _contact.rigidbody[1] != nullptr ){ totalInverseMass += _contact.rigidbody[1]->mInverseMass; }
        const Fixed separatingVelocity        = ContactSolver::CalculateSeparatingVelocity( _contact );
        const Fixed desiredTotalDeltaVelocity = -separatingVelocity * ( 1 + _contact.restitution ) / totalInverseMass;

        const Fixed   deltaVelocityPerUnitImpulse0 = ContactSolver::CalculateDeltaVelocityPerUnitImpulse( _contact, 0 );
        const Fixed   desiredDeltaVelocity0        = _contact.rigidbody[0]->mInverseMass * desiredTotalDeltaVelocity;
        const Vector3 impulseContact( desiredDeltaVelocity0 / deltaVelocityPerUnitImpulse0, 0, 0 );
        const Vector3 impulse                      = _contact.contactToWorld * impulseContact;
        const Vector3 velocityChange               = impulse * _contact.rigidbody[0]->mInverseMass;
        const Vector3 impulsiveTorque              = Vector3::Cross( _contact.relativeContactPosition[0], impulse );
        const Vector3 rotationChange               = _contact.rigidbody[0]->mInverseInertiaTensorWorld * impulsiveTorque;

        _contact.rigidbody[0]->mVelocity += velocityChange;
        _contact.rigidbody[0]->mRotation += rotationChange;

        if( _contact.rigidbody[1] )
        {
            const Fixed   desiredDeltaVelocity1        = -_contact.rigidbody[1]->mInverseMass * desiredTotalDeltaVelocity;
            const Fixed   deltaVelocityPerUnitImpulse1 = ContactSolver::CalculateDeltaVelocityPerUnitImpulse( _contact, 1 );
            const Vector3 impulseContact1( desiredDeltaVelocity1 / deltaVelocityPerUnitImpulse1, 0, 0 );
            const Vector3 impulse1                     = _contact.contactToWorld * impulseContact1;
            const Vector3 velocityChange1              = impulse1 * _contact.rigidbody[1]->mInverseMass;
            const Vector3 impulsiveTorque1             = Vector3::Cross( _contact.relativeContactPosition[1], impulse1 );
            const Vector3 rotationChange1              = _contact.rigidbody[1]->mInverseInertiaTensorWorld * impulsiveTorque1;

            _contact.rigidbody[1]->mVelocity += velocityChange1;
            _contact.rigidbody[1]->mRotation += rotationChange1;
        }
    }

    //==================================================================================================================================================================================================
    // non linear projection
    //==================================================================================================================================================================================================
    void ContactSolver::ResolveInterpenetration( Contact& _contact ) const
    {
        Fixed       angularInertia[2];
        Fixed       linearInertia[2];
        Fixed       totalInertia = 0;
        for( int    i                   = 0; i < 2; i++ )
        {
            // calculates the angular/linear inertia of each objects to determine the ratio of rotation/translation used for solving interpenetration
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
            const Fixed objectSize = _contact.relativeContactPosition->Magnitude();
            const Fixed angularLimit = mAngularLimitNonLinearProjection * objectSize;
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
                FxRigidbody& rb = *_contact.rigidbody[i];
                rb.mTransform->mPosition += _contact.normal * linearMove[i]; // translate to solve interpenetration

                if( angularInertia[i] != 0)
                {
                    // get the amount of rotation for a unit move
                    const Vector3 torquePerMove   = Vector3::Cross( _contact.relativeContactPosition[i], _contact.normal );
                    const Vector3 impulsePerMove  = rb.mInverseInertiaTensorWorld * torquePerMove;
                    Vector3       rotationPerMove = impulsePerMove / angularInertia[i];
                    Vector3       rotation        = angularMove[i] * rotationPerMove;

                    // rotate to solve interpenetration
                    Fixed magnitude = rotation.Magnitude();
                    if( !Fixed::IsFuzzyZero( magnitude ) )
                    {
                        rb.mTransform->mRotation =  rb.mTransform->mRotation + FIXED( 0.5 ) * Quaternion( 0, rotation ) *  rb.mTransform->mRotation;
                        rb.mTransform->mRotation.Normalize();
                    }
                }
            }
        }
    }
}