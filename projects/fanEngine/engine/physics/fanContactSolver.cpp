#include "core/fanDebug.hpp"
#include "core/time/fanProfiler.hpp"
#include "engine/physics/fanContactSolver.hpp"
#include "engine/physics/fanRigidbody.hpp"
#include "fanTransform.hpp"
#include "engine/singletons/fanRenderDebug.hpp"

namespace fan
{
    RenderDebug* ContactSolver::tmpRd;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ContactSolver::ResolveContacts( std::vector<Contact> _contacts, const Fixed _deltaTime )
    {
        if( _contacts.empty() ){ return; }

        SCOPED_PROFILE( resolve_contacts )

        PrepareContacts( _contacts, _deltaTime );
        ResolvePositions( _contacts, _deltaTime );
        ResolveVelocities( _contacts, _deltaTime );
    }

    //==================================================================================================================================================================================================
    //==============================================================================f====================================================================================================================
    void ContactSolver::PrepareContacts( std::vector<Contact>& _contacts, const Fixed _deltaTime )
    {
        SCOPED_PROFILE( prepare_contacts )
        for( int i = (int)_contacts.size() - 1; i >= 0; --i )
        {
            Contact& contact = _contacts[i];

            // remove sleeping contacts
            if( contact.rigidbody[0]->mIsSleeping )
            {
                if( contact.rigidbody[1] == nullptr || contact.rigidbody[1]->mIsSleeping )
                {
                    _contacts[i] = _contacts[_contacts.size() - 1];
                    _contacts.pop_back();
                    continue;
                }
            }

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
    void MatchSleepState( Contact& _contact )
    {
        if( !_contact.rigidbody[1] ){ return; }
        if( _contact.rigidbody[0]->mIsSleeping ){ _contact.rigidbody[0]->SetSleeping( false ); }
        if( _contact.rigidbody[1]->mIsSleeping ){ _contact.rigidbody[1]->SetSleeping( false ); }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ContactSolver::ResolvePositions( std::vector<Contact>& _contacts, const Fixed _deltaTime )
    {
        if( _deltaTime == 0 ){ return; }

        SCOPED_PROFILE( resolve_positions )

        for( mPositionIterationsUsed = 0; mPositionIterationsUsed < mMaxPositionsIterations * _contacts.size(); mPositionIterationsUsed++ )
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

            MatchSleepState( *worstContact );

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
                }
                else if( contact.rigidbody[0] == worstContact->rigidbody[1] )
                {
                    const Vector3 deltaPosition    = velocityChange[1];
                    const Vector3 deltaRotation    = Vector3::Cross( rotationChange[1], contact.relativeContactPosition[0] );
                    const Fixed   deltaPenetration = Vector3::Dot( deltaRotation + deltaPosition, contact.normal );
                    contact.penetration -= deltaPenetration;
                }
                if( contact.rigidbody[1] )
                {
                    if( contact.rigidbody[1] == worstContact->rigidbody[0] )
                    {
                        const Vector3 deltaPosition    = velocityChange[0];
                        const Vector3 deltaRotation    = Vector3::Cross( rotationChange[0], contact.relativeContactPosition[1] );
                        const Fixed   deltaPenetration = Vector3::Dot( deltaRotation + deltaPosition, contact.normal );
                        contact.penetration += deltaPenetration;
                    }
                    else if( contact.rigidbody[1] == worstContact->rigidbody[1] )
                    {
                        const Vector3 deltaPosition    = velocityChange[1];
                        const Vector3 deltaRotation    = Vector3::Cross( rotationChange[1], contact.relativeContactPosition[1] );
                        const Fixed   deltaPenetration = Vector3::Dot( deltaRotation + deltaPosition, contact.normal );
                        contact.penetration += deltaPenetration;
                    }
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ContactSolver::ResolveVelocities( std::vector<Contact>& _contacts, const Fixed _deltaTime )
    {
        SCOPED_PROFILE( resolve_velocities )

        for( mVelocityIterationsUsed = 0; mVelocityIterationsUsed < mMaxVelocityIterations * _contacts.size(); mVelocityIterationsUsed++ )
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

            ResolveVelocity( *worstContact, _deltaTime, mFriction );

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
        //SCOPED_PROFILE(resolve_position)

        Fixed    angularInertia[2];
        Fixed    linearInertia[2];
        Fixed    totalInertia = 0;
        for( int i            = 0; i < 2; i++ )
        {
            // calculates the angular/linear inertia of each objects to determine the ratio of rotation/translation used for solving interpenetration
            if( _contact.rigidbody[i] )
            {
                const Rigidbody& rb = *_contact.rigidbody[i];
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
                Rigidbody& rb = *_contact.rigidbody[i];
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
    void ContactSolver::ResolveVelocity( const Contact& _contact, const Fixed _deltaTime, const Fixed _friction )
    {
        //SCOPED_PROFILE(resolve_velocity)

        // build a matrix to convert contact impulse into velocity change
        const Matrix3 impulseToTorque        = Matrix3::SkewSymmetric( _contact.relativeContactPosition[0] );
        const Matrix3 rotationPerUnitImpulse = _contact.rigidbody[0]->mInverseInertiaTensorWorld * impulseToTorque;
        Matrix3       velocityPerUnitImpulse = -impulseToTorque * rotationPerUnitImpulse;
        if( _contact.rigidbody[1] )
        {
            const Matrix3 impulseToTorque1        = Matrix3::SkewSymmetric( _contact.relativeContactPosition[1] );
            const Matrix3 rotationPerUnitImpulse1 = _contact.rigidbody[1]->mInverseInertiaTensorWorld * impulseToTorque1;
            Matrix3       velocityPerUnitImpulse1 = -impulseToTorque1 * rotationPerUnitImpulse1;

            velocityPerUnitImpulse += velocityPerUnitImpulse1;
        }

        // add linear velocity change
        velocityPerUnitImpulse.e11 += _contact.totalInverseMass;
        velocityPerUnitImpulse.e22 += _contact.totalInverseMass;
        velocityPerUnitImpulse.e33 += _contact.totalInverseMass;

        const Matrix3 velocityPerUnitImpulseContact = _contact.contactToWorld.Transpose() * velocityPerUnitImpulse * _contact.contactToWorld;
        const Matrix3 impulsePerUnitVelocityContact = velocityPerUnitImpulseContact.Inverse();
        Vector3       impulseFriction               = impulsePerUnitVelocityContact * Vector3( _contact.desiredTotalDeltaVelocity, -_contact.relativeVelocity.y, -_contact.relativeVelocity.z );
        const Fixed   reactionImpulse               = Fixed::Abs( impulseFriction.x );
        impulseFriction.x = 0;

        // use dynamic friction when friction exceeds a certain threshold
        const Fixed planarImpulse = impulseFriction.Magnitude();
        if( planarImpulse > reactionImpulse * _friction )
        {
            impulseFriction.y /= planarImpulse;
            impulseFriction.z /= planarImpulse;
            impulseFriction *= _friction * reactionImpulse;
        }

        // apply impulse to first body
        if( _contact.rigidbody[0]->mInverseMass != 0 )
        {
            const Fixed   deltaVelocityPerUnitImpulse0 = ContactSolver::CalculateDeltaVelocityPerUnitImpulse( _contact, 0 );
            const Fixed   desiredDeltaVelocity0        = _contact.rigidbody[0]->mInverseMass * _contact.desiredTotalDeltaVelocity / _contact.totalInverseMass;
            const Vector3 deltaVelocityFriction        = _contact.rigidbody[0]->mInverseMass * impulseFriction / _contact.totalInverseMass;
            const Vector3 impulseContact               = Vector3( desiredDeltaVelocity0 / deltaVelocityPerUnitImpulse0, 0, 0 ) + deltaVelocityFriction;
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
            const Vector3 deltaVelocityFriction1       = _contact.rigidbody[1]->mInverseMass * impulseFriction / _contact.totalInverseMass;
            const Vector3 impulseContact1              = Vector3( desiredDeltaVelocity1 / deltaVelocityPerUnitImpulse1, 0, 0 ) + deltaVelocityFriction1;
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