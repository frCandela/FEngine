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
            if( separatingVelocity < maxSeparatingVelocity )
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
        // calculate closing velocity
        Vector3 closingVelocity = Vector3::Cross( _contact.relativeContactPosition0, _contact.rb0->mRotation );
        closingVelocity += _contact.rb0->mVelocity;
        if( _contact.rb1 )
        {
            closingVelocity -= Vector3::Cross( _contact.rb1->mRotation, _contact.relativeContactPosition1 );
            closingVelocity -= _contact.rb1->mVelocity;
        }
        closingVelocity = _contact.contactToWorld.Transpose() * closingVelocity;
        return closingVelocity.x;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ContactSolver::ResolveVelocity( const Contact& _contact, Fixed _duration )
    {
        (void)_duration;

        // calculate closing velocity
        Vector3 closingVelocity = Vector3::Cross( _contact.relativeContactPosition0, _contact.rb0->mRotation );
        closingVelocity += _contact.rb0->mVelocity;
        if( _contact.rb1 )
        {
            closingVelocity -= Vector3::Cross( _contact.rb1->mRotation, _contact.relativeContactPosition1 );
            closingVelocity -= _contact.rb1->mVelocity;
        }
        closingVelocity = _contact.contactToWorld.Transpose() * closingVelocity;
        Fixed desiredDeltaVelocity = -closingVelocity.x * ( 1 + _contact.restitution);

        // Build a vector that shows the change in velocity in world space for a unit impulse in the direction of the contact normal.
        Vector3 deltaVelWorld           = Vector3::Cross( _contact.relativeContactPosition0, _contact.normal );
        deltaVelWorld = _contact.rb0->mInverseInertiaTensorWorld * deltaVelWorld;
        deltaVelWorld = Vector3::Cross( deltaVelWorld, _contact.relativeContactPosition0 );
        deltaVelWorld = _contact.contactToWorld.Transpose() * deltaVelWorld;
        Fixed deltaVelocity = deltaVelWorld.x; // angular component

        deltaVelocity += _contact.rb0->mInverseMass;  // linear component

        // impulse
        Vector3 impulseContact( desiredDeltaVelocity / deltaVelocity, 0, 0 );
        Vector3 impulse = _contact.contactToWorld * impulseContact;
        Vector3 velocityChange = impulse * _contact.rb0->mInverseMass;
        Vector3 impulsiveTorque = Vector3::Cross( impulse, _contact.relativeContactPosition0 );
        Vector3 rotationChange = _contact.rb0->mInverseInertiaTensorWorld * impulsiveTorque;

        _contact.rb0->mVelocity += velocityChange;
        _contact.rb0->mRotation += rotationChange;

        if (_contact.rb1)
        {
            desiredDeltaVelocity = -desiredDeltaVelocity;

            // Build a vector that shows the change in velocity in world space for a unit impulse in the direction of the contact normal.
            Vector3 deltaVelWorld1          = Vector3::Cross( _contact.relativeContactPosition1, _contact.normal );
            deltaVelWorld1 = _contact.rb1->mInverseInertiaTensorWorld * deltaVelWorld1;
            deltaVelWorld1 = Vector3::Cross( deltaVelWorld1, _contact.relativeContactPosition1 );
            deltaVelWorld1 = _contact.contactToWorld.Transpose() * deltaVelWorld1;
            Fixed deltaVelocity1 = deltaVelWorld1.x; // angular component

            deltaVelocity1 += _contact.rb1->mInverseMass;  // linear component

            // impulse
            Vector3 impulseContact1( desiredDeltaVelocity / deltaVelocity1, 0, 0 );
            Vector3 impulse1 = _contact.contactToWorld * impulseContact1;
            Vector3 velocityChange1 = impulse1 * _contact.rb1->mInverseMass;
            Vector3 impulsiveTorque1 = Vector3::Cross( impulse1, _contact.relativeContactPosition1 );
            Vector3 rotationChange1 = _contact.rb1->mInverseInertiaTensorWorld * impulsiveTorque1;
            _contact.rb1->mVelocity += velocityChange1;
            _contact.rb1->mRotation += rotationChange1;
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