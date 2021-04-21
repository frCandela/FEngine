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
        Fixed mAngularLimitNonLinearProjection = FIXED(0.2);

        void ResolveContacts( std::vector<Contact> _contacts);
        static Fixed CalculateSeparatingVelocity( const Contact& _contact );
        static void ResolveVelocity( const Contact& _contact );
        void ResolveInterpenetration( Contact& _contact ) const;
        static Fixed CalculateDeltaVelocityPerUnitImpulse( const Contact& _contact, const int _index );
    };
}