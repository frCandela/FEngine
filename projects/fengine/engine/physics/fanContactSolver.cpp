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
    void ContactSolver::ResolveContacts( std::vector<Contact> _contacts )
    {
        if( _contacts.empty() ){ return; }

        PrepareContacts( _contacts );
        AdjustPositions( _contacts );
        //ResolveVelocity( _contacts );



        /*while( !_contacts.empty() )
        {
            // find the contact with the largest closing velocity
            Fixed    maxSeparatingVelocity = 0;
            int      maxIndex              = (int)_contacts.size();
            for( int i                     = 0; i < (int)_contacts.size(); ++i )
            {
                const Fixed separatingVelocity = _contacts[i].relativeVelocity.x;
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

            ResolveInterpenetration( contact );
            ResolveVelocity( contact );
        }*/
    }

    //==================================================================================================================================================================================================
    //==============================================================================f====================================================================================================================
    void ContactSolver::PrepareContacts( std::vector<Contact>& _contacts )
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

            // relative contact velocity
            contact.relativeVelocity = Vector3::Cross( contact.rigidbody[0]->mRotation, contact.relativeContactPosition[0] );
            contact.relativeVelocity += contact.rigidbody[0]->mVelocity;
            if( contact.rigidbody[1] )
            {
                contact.relativeVelocity -= Vector3::Cross( contact.rigidbody[1]->mRotation, contact.relativeContactPosition[1] );
                contact.relativeVelocity -= contact.rigidbody[1]->mVelocity;
            }
            contact.relativeVelocity = contact.contactToWorld.Transpose() * contact.relativeVelocity;

            // desired delta velocity
            contact.desiredTotalDeltaVelocity = -contact.relativeVelocity.x * ( 1 + contact.restitution ) / contact.totalInverseMass;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ContactSolver::AdjustPositions( std::vector<Contact>& _contacts )
    {
        for( mPositionIterationsUsed = 0; mPositionIterationsUsed < mMaxPositionsIterations; mPositionIterationsUsed++ )
        {
            Contact* worstContact = nullptr;
            Fixed worstPenetration = 0;
            for( Contact& contact : _contacts )
            {
                if( contact.penetration > worstPenetration )
                {
                    worstContact     = &contact;
                    worstPenetration = contact.penetration;
                }
            }

            if( !worstContact ){ break; }

            Vector3 rotationChange[2], velocityChange[2];
            ResolveInterpenetration( *worstContact, mAngularLimitNonLinearProjection, rotationChange, velocityChange );

            // update penetration for all contacts
            for( Contact& contact : _contacts )
            {
                if( contact.rigidbody[0] == worstContact->rigidbody[0] )
                {
                    const Vector3 deltaPosition = velocityChange[0];
                    const Vector3 deltaRotation = Vector3::Cross( rotationChange[0], contact.relativeContactPosition[0] );
                    const Fixed   deltaPenetration = Vector3::Dot( deltaRotation + deltaPosition, contact.normal );
                    contact.penetration -= deltaPenetration;

                    tmpRd->DebugLine( contact.position, contact.position +  deltaRotation, Color::sBlue );
                    tmpRd->DebugLine( contact.position + deltaRotation, contact.position + deltaRotation + deltaPosition, Color::sCyan );
                    tmpRd->DebugLine( contact.position, contact.position + deltaRotation + deltaPosition, Color::sPurple );
                }
                else if( contact.rigidbody[0] == worstContact->rigidbody[1] )
                {
                    const Vector3 deltaPosition = velocityChange[1];
                    const Vector3 deltaRotation = Vector3::Cross( rotationChange[1], contact.relativeContactPosition[0] );
                    const Fixed   deltaPenetration = Vector3::Dot( deltaRotation + deltaPosition, contact.normal );
                    contact.penetration -= deltaPenetration;

                    tmpRd->DebugLine( contact.position, contact.position +  deltaRotation, Color::sBlue );
                    tmpRd->DebugLine( contact.position + deltaRotation, contact.position + deltaRotation + deltaPosition, Color::sCyan );
                    tmpRd->DebugLine( contact.position, contact.position + deltaRotation + deltaPosition, Color::sPurple );
                }
                if( contact.rigidbody[1] )
                {
                    if( contact.rigidbody[1] == worstContact->rigidbody[0] )
                    {
                        const Vector3 deltaPosition = velocityChange[0];
                        const Vector3 deltaRotation = Vector3::Cross( rotationChange[0], contact.relativeContactPosition[1] );
                        const Fixed   deltaPenetration = Vector3::Dot( deltaRotation + deltaPosition, contact.normal );
                        contact.penetration += deltaPenetration;

                        tmpRd->DebugLine( contact.position, contact.position +  deltaRotation, Color::sBlue );
                        tmpRd->DebugLine( contact.position + deltaRotation, contact.position + deltaRotation + deltaPosition, Color::sCyan );
                        tmpRd->DebugLine( contact.position, contact.position + deltaRotation + deltaPosition, Color::sPurple );
                    }
                    else if( contact.rigidbody[1] == worstContact->rigidbody[1] )
                    {
                        const Vector3 deltaPosition = velocityChange[1];
                        const Vector3 deltaRotation = Vector3::Cross( rotationChange[1], contact.relativeContactPosition[1] );
                        const Fixed   deltaPenetration = Vector3::Dot( deltaRotation + deltaPosition, contact.normal );
                        contact.penetration += deltaPenetration;

                        tmpRd->DebugLine( contact.position, contact.position +  deltaRotation, Color::sBlue );
                        tmpRd->DebugLine( contact.position + deltaRotation, contact.position + deltaRotation + deltaPosition, Color::sCyan );
                        tmpRd->DebugLine( contact.position, contact.position + deltaRotation + deltaPosition, Color::sPurple );
                    }
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    // non linear projection
    //==================================================================================================================================================================================================
    void ContactSolver::ResolveInterpenetration( Contact& _contact, const Fixed _angularLimitNonLinearProjection, Vector3 _outRotationChange[2], Vector3 _outVelocityChange[2] )
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
                //rb.mTransform->mPosition += velocityChange; // translate to solve interpenetration
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
                        //rb.mTransform->mRotation = rb.mTransform->mRotation + FIXED( 0.5 ) * Quaternion( 0, rotationChange ) * rb.mTransform->mRotation;
                        //rb.mTransform->mRotation.Normalize();
                    }
                }
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
    void ContactSolver::ResolveVelocity( const Contact& _contact )
    {
        const Fixed   deltaVelocityPerUnitImpulse0 = ContactSolver::CalculateDeltaVelocityPerUnitImpulse( _contact, 0 );
        const Fixed   desiredDeltaVelocity0        = _contact.rigidbody[0]->mInverseMass * _contact.desiredTotalDeltaVelocity;
        const Vector3 impulseContact( desiredDeltaVelocity0 / deltaVelocityPerUnitImpulse0, 0, 0 );
        const Vector3 impulse                      = _contact.contactToWorld * impulseContact;
        const Vector3 velocityChange               = impulse * _contact.rigidbody[0]->mInverseMass;
        const Vector3 impulsiveTorque              = Vector3::Cross( _contact.relativeContactPosition[0], impulse );
        const Vector3 rotationChange               = _contact.rigidbody[0]->mInverseInertiaTensorWorld * impulsiveTorque;

        _contact.rigidbody[0]->mVelocity += velocityChange;
        _contact.rigidbody[0]->mRotation += rotationChange;

        if( _contact.rigidbody[1] )
        {
            const Fixed   desiredDeltaVelocity1        = -_contact.rigidbody[1]->mInverseMass * _contact.desiredTotalDeltaVelocity;
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
}