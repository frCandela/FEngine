#pragma once

#include <vector>
#include "engine/physics/fanContact.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct ContactSolver
    {
        int mMaxIterations = 50;
        int mIterationUsed;

        void ResolveContacts( std::vector<Contact> _contacts, const Fixed _duration );
        static Fixed CalculateSeparatingVelocity( const Contact& _contact );
        static void ResolveVelocity( const Contact& _contact, Fixed _duration );
        static void ResolveInterpenetration( Contact& _contact, Fixed _duration );
        static Fixed CalculateDeltaVelocityPerUnitImpulse( const Contact& _contact, const int _index );
    };
}