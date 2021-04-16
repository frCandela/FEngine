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
        Vector3 separatingVelocity = Vector3::Cross( _contact.rb0->mRotation, _contact.relativeContactPosition0 );
        separatingVelocity += _contact.rb0->mVelocity;
        if( _contact.rb1 )
        {
            separatingVelocity -= Vector3::Cross( _contact.rb1->mRotation, _contact.relativeContactPosition1 );
            separatingVelocity -= _contact.rb1->mVelocity;
        }
        separatingVelocity = _contact.contactToWorld.Transpose() * separatingVelocity;
        return separatingVelocity.x;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ContactSolver::ResolveVelocity( const Contact& _contact, Fixed _duration )
    {
        (void)_duration;

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

        _contact.rb0->mVelocity += velocityChange;
        _contact.rb0->mRotation += rotationChange;

        if (_contact.rb1)
        {
            desiredDeltaVelocity = -desiredDeltaVelocity;


        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ContactSolver::ResolveInterpenetration( Contact& _contact, Fixed _duration )
    {
        (void)_duration;

        if( _contact.penetration <= 0 ){ return; }

        Fixed totalInverseMass = _contact.rb0->mInverseMass;
        if( _contact.rb1 )
        {
            totalInverseMass += _contact.rb1->mInverseMass;
        }
        if( totalInverseMass <= 0 ){ return; }// infinite mass => immovable

        Vector3 movePerInvMass = _contact.normal * _contact.penetration / totalInverseMass;

        float m = _contact.rb0->mInverseMass.ToFloat();        (void)m;
        float pen = _contact.penetration.ToFloat();        (void)pen;
        glm::vec3 move = Math::ToGLM(movePerInvMass * _contact.rb0->mInverseMass);        (void)move;

        _contact.transform0->mPosition = _contact.transform0->mPosition + movePerInvMass * _contact.rb0->mInverseMass;
        glm::vec3 p = Math::ToGLM( _contact.transform0->mPosition ); (void)p;
        if( _contact.rb1 )
        {
            _contact.transform1->mPosition = _contact.transform1->mPosition - movePerInvMass * _contact.rb1->mInverseMass;
        }
    }
}