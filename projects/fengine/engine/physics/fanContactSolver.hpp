#pragma once

#include <vector>
#include "engine/physics/fanContact.hpp"

namespace fan
{
    struct RenderDebug;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct ContactSolver
    {
        RenderDebug* tmpRd;

        int   mMaxPositionsIterations          = 50;
        int   mMaxIterations                   = 50;
        int   mIterationsUsed;
        int   mPositionIterationsUsed;

        Fixed mAngularLimitNonLinearProjection = FIXED( 0.2 );
        Fixed mRestitution                     = FIXED( 0.3 ); // [0,1]

        void ResolveContacts( std::vector<Contact> _contacts );
        void PrepareContacts( std::vector<Contact>& _contacts );
        void AdjustPositions( std::vector<Contact>& _contacts );
        static void ResolveVelocity( const Contact& _contact );

        static void ResolveInterpenetration( Contact& _contact, const Fixed _angularLimitNonLinearProjection, Vector3 _outRotationChange[2], Vector3 _outVelocityChange[2] );

        static Fixed CalculateDeltaVelocityPerUnitImpulse( const Contact& _contact, const int _index );
    };
}