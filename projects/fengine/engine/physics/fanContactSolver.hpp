#pragma once

#include <vector>
#include "engine/physics/fanContact.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct ContactSolver
    {
        int mMaxIterations = 10;
        int mIterationUsed;

        void ResolveContacts( const std::vector<Contact>& _contacts, const Fixed _duration );
        static Fixed CalculateSeparatingVelocity( const Contact& _contact );
        static void ResolveVelocity( const Contact& _contact, Fixed _duration );
        static void ResolveInterpenetration( const Contact& _contact, Fixed _duration );
    };
}