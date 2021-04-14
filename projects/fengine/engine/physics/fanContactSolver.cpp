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

        Contact& contact = _contacts[maxIndex];
        ResolveVelocity( contact, _duration );
        ResolveInterpenetration( contact, _duration );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Fixed ContactSolver::CalculateSeparatingVelocity( const Contact& _contact )
    {
        Vector3 relativeVelocity = _contact.rb0->mVelocity;
        if( _contact.rb1 ){ relativeVelocity -= _contact.rb1->mVelocity; }
        return Vector3::Dot( relativeVelocity, _contact.normal );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ContactSolver::ResolveVelocity( const Contact& contact, Fixed _duration )
    {
        (void)_duration;

        // calculate closing velocity
        Vector3 closingVelocity = Vector3::Cross( contact.rb0->mRotation, contact.relativeContactPosition0 );
        closingVelocity += contact.rb0->mVelocity;
        if( contact.rb1 )
        {
            closingVelocity -= Vector3::Cross( contact.rb1->mRotation, contact.relativeContactPosition1 );
            closingVelocity -= contact.rb1->mVelocity;
        }
        closingVelocity = contact.contactToWorld.Transpose() * closingVelocity;
        Fixed desiredDeltaVelocity = -closingVelocity.x * (1 + contact.restitution);

        // Build a vector that shows the change in velocity in world space for a unit impulse in the direction of the contact normal.
        Vector3 deltaVelWorld           = Vector3::Cross( contact.relativeContactPosition0, contact.normal );
        deltaVelWorld = contact.rb0->mInverseInertiaTensorWorld * deltaVelWorld;
        deltaVelWorld = Vector3::Cross( deltaVelWorld, contact.relativeContactPosition0 );
        deltaVelWorld = contact.contactToWorld.Transpose() * deltaVelWorld;
        Fixed deltaVelocity = deltaVelWorld.x; // angular component

        deltaVelocity += contact.rb0->mInverseMass;  // linear component

        // impulse
        Vector3 impulseContact( desiredDeltaVelocity / deltaVelocity, 0, 0 );
        Vector3 impulse = contact.contactToWorld * impulseContact;
        Vector3 velocityChange = impulse * contact.rb0->mInverseMass;
        Vector3 impulsiveTorque = Vector3::Cross( impulse, contact.relativeContactPosition0 );
        Vector3 rotationChange = contact.rb0->mInverseInertiaTensorWorld * impulsiveTorque;


        //float glmDesired = desiredDeltaVelocity.ToFloat(); (void)glmDesired;
        glm::vec3 glmcPos = Math::ToGLM(contact.relativeContactPosition0);(void)glmcPos;
        glm::vec3 glmTorque = Math::ToGLM(impulsiveTorque);(void)glmTorque;

        contact.rb0->mVelocity += velocityChange;
        contact.rb0->mRotation += rotationChange;

        if (contact.rb1)
        {
            desiredDeltaVelocity = -desiredDeltaVelocity;

            // Build a vector that shows the change in velocity in world space for a unit impulse in the direction of the contact normal.
            Vector3 deltaVelWorld1          = Vector3::Cross( contact.relativeContactPosition1, contact.normal );
            deltaVelWorld1 = contact.rb1->mInverseInertiaTensorWorld * deltaVelWorld1;
            deltaVelWorld1 = Vector3::Cross( deltaVelWorld1, contact.relativeContactPosition1 );
            deltaVelWorld1 = contact.contactToWorld.Transpose() * deltaVelWorld1;
            Fixed deltaVelocity1 = deltaVelWorld1.x; // angular component

            deltaVelocity1 += contact.rb1->mInverseMass;  // linear component

            // impulse
            Vector3 impulseContact1( desiredDeltaVelocity / deltaVelocity1, 0, 0 );
            Vector3 impulse1 = contact.contactToWorld * impulseContact1;
            Vector3 velocityChange1 = impulse1 * contact.rb1->mInverseMass;
            Vector3 impulsiveTorque1 = Vector3::Cross( impulse1, contact.relativeContactPosition1 );
            Vector3 rotationChange1 = contact.rb1->mInverseInertiaTensorWorld * impulsiveTorque1;
            contact.rb1->mVelocity += velocityChange1;
            contact.rb1->mRotation += rotationChange1;
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