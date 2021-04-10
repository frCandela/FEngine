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
       /* mIterationUsed = 0;
        while( mIterationUsed < mMaxIterations )
        {
            for( int i = 0; i < _contacts.size(); i++ )
            {
                ResolveInterpenetration( _contacts[i], _duration );
            }
            mIterationUsed++;
        }*/


        //mIterationUsed = 0;
        //( mIterationUsed < mMaxIterations )
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
                //break;
            }

            Contact& contact = _contacts[maxIndex];
            ResolveVelocity( contact, _duration );
            ResolveInterpenetration( contact, _duration );

            //mIterationUsed++;
        }
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
    void ContactSolver::ResolveVelocity( const Contact& _contact, Fixed _duration )
    {
        const Fixed separatingVelocity = CalculateSeparatingVelocity( _contact );
        if( separatingVelocity > 0 ){ return; }

        Fixed newSeparatingVelocity = -separatingVelocity * _contact.restitution;

        // check the velocity build up due to acceleration only (causes vibrations on resting contacts)
        Vector3 accCausedVelocity = _contact.rb0->mAcceleration;
        if( _contact.rb1 ){ accCausedVelocity -= _contact.rb1->mAcceleration; }
        const Fixed accCausedSeparatingVelocity = _duration * Vector3::Dot( accCausedVelocity, _contact.normal );
        if( accCausedSeparatingVelocity < 0 )
        {
            newSeparatingVelocity += _contact.restitution * accCausedSeparatingVelocity;
            if( newSeparatingVelocity < 0 ){ newSeparatingVelocity = 0; }
        }

        Fixed totalInverseMass = _contact.rb0->mInverseMass;
        if( _contact.rb1 )
        {
            totalInverseMass += _contact.rb1->mInverseMass;
        }
        if( totalInverseMass <= 0 ){ return; }// infinite mass => immovable

        const Fixed deltaVelocity  = newSeparatingVelocity - separatingVelocity;
        Vector3     impulsePerMass = ( deltaVelocity / totalInverseMass ) * _contact.normal;
        _contact.rb0->mVelocity = _contact.rb0->mVelocity + _contact.rb0->mInverseMass * impulsePerMass;
        if( _contact.rb1 )
        {
            _contact.rb1->mVelocity = _contact.rb1->mVelocity - _contact.rb1->mInverseMass * impulsePerMass;
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