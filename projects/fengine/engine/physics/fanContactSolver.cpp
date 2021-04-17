#include <core/math/fanMathUtils.hpp>
#include "engine/physics/fanContactSolver.hpp"

#include "engine/physics/fanFxRigidbody.hpp"
#include "engine/components/fanFxTransform.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ContactSolver::ResolveContacts( std::vector<Contact> _contacts, const Fixed _duration )
    {
        // find the contact with the largest closing velocity
        Fixed maxSeparatingVelocity = 0;
        int   maxIndex              = (int)_contacts.size();
        for( int i = 0; i < (int)_contacts.size(); ++i )
        {
            const Fixed separatingVelocity = CalculateSeparatingVelocity( _contacts[i] );
            if( separatingVelocity < 0 && separatingVelocity < maxSeparatingVelocity )
            {
                maxSeparatingVelocity = separatingVelocity;
                maxIndex              = i;
            }
        }

        if( maxIndex == (int)_contacts.size() )
        {
            return;
        }
        if( _contacts.empty()){ return; }

        Contact& contact = _contacts[0];
        ResolveVelocity( contact, _duration );
        ResolveInterpenetration( contact, _duration );
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
    void ContactSolver::ResolveVelocity( const Contact& _contact, Fixed _duration )
    {
        (void)_duration;

        Fixed totalInverseMass = _contact.rigidbody[0]->mInverseMass;
        if( _contact.rigidbody[1] != nullptr ){ totalInverseMass += _contact.rigidbody[1]->mInverseMass; }
       const Fixed separatingVelocity = ContactSolver::CalculateSeparatingVelocity(_contact);
       const Fixed desiredTotalDeltaVelocity = -separatingVelocity * ( 1 + _contact.restitution) / totalInverseMass;


        const Fixed deltaVelocityPerUnitImpulse0 = ContactSolver::CalculateDeltaVelocityPerUnitImpulse(_contact, 0);
        const Fixed desiredDeltaVelocity0 = _contact.rigidbody[0]->mInverseMass * desiredTotalDeltaVelocity;
        const Vector3 impulseContact( desiredDeltaVelocity0 / deltaVelocityPerUnitImpulse0, 0, 0 );
        const Vector3 impulse = _contact.contactToWorld * impulseContact;
        const Vector3 velocityChange = impulse * _contact.rigidbody[0]->mInverseMass;
        const Vector3 impulsiveTorque = Vector3::Cross( _contact.relativeContactPosition[0], impulse );
        const Vector3 rotationChange = _contact.rigidbody[0]->mInverseInertiaTensorWorld * impulsiveTorque;

        _contact.rigidbody[0]->mVelocity += velocityChange;
        _contact.rigidbody[0]->mRotation += rotationChange;

        if (_contact.rigidbody[1])
        {
            const Fixed desiredDeltaVelocity1 = -_contact.rigidbody[1]->mInverseMass * desiredTotalDeltaVelocity;
            const Fixed deltaVelocityPerUnitImpulse1 = ContactSolver::CalculateDeltaVelocityPerUnitImpulse(_contact, 1);
            const Vector3 impulseContact1( desiredDeltaVelocity1 / deltaVelocityPerUnitImpulse1, 0, 0 );
            const Vector3 impulse1 = _contact.contactToWorld * impulseContact1;
            const Vector3 velocityChange1 = impulse1 * _contact.rigidbody[1]->mInverseMass;
            const Vector3 impulsiveTorque1 = Vector3::Cross( _contact.relativeContactPosition[1], impulse1 );
            const Vector3 rotationChange1 = _contact.rigidbody[1]->mInverseInertiaTensorWorld * impulsiveTorque1;

            _contact.rigidbody[1]->mVelocity += velocityChange1;
            _contact.rigidbody[1]->mRotation += rotationChange1;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ContactSolver::ResolveInterpenetration( Contact& _contact, Fixed _duration )
    {
        (void)_duration;

        if( _contact.penetration <= 0 ){ return; }

        Fixed totalInverseMass = _contact.rigidbody[0]->mInverseMass;
        if( _contact.rigidbody[1] )
        {
            totalInverseMass += _contact.rigidbody[1]->mInverseMass;
        }
        if( totalInverseMass <= 0 ){ return; }// infinite mass => immovable

        Vector3 movePerInvMass = _contact.normal * _contact.penetration / totalInverseMass;
        _contact.transform[0]->mPosition = _contact.transform[0]->mPosition + movePerInvMass * _contact.rigidbody[0]->mInverseMass;
        if( _contact.rigidbody[1] )
        {
            _contact.transform[1]->mPosition = _contact.transform[1]->mPosition - movePerInvMass * _contact.rigidbody[1]->mInverseMass;
        }
    }
}