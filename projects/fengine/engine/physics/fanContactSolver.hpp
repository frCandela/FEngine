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

        int mMaxPositionsIterations = 50;
        int mMaxVelocityIterations  = 50;
        int mVelocityIterationsUsed;
        int mPositionIterationsUsed;

        Fixed mAngularLimitNonLinearProjection = FIXED( 0.2 );
        Fixed mRestitution                     = FIXED( 0.3 ); // [0,1]

        void ResolveContacts( std::vector<Contact> _contacts );
        void PrepareContacts( std::vector<Contact>& _contacts );
        void ResolvePositions( std::vector<Contact>& _contacts );
        void ResolveVelocities( std::vector<Contact>& _contacts );

        static void ResolveVelocity( const Contact& _contact );
        static void ResolvePosition( Contact& _contact, const Fixed _angularLimitNonLinearProjection, Vector3* _outRotationChange, Vector3* _outVelocityChange );

        static Fixed CalculateDeltaVelocityPerUnitImpulse( const Contact& _contact, const int _index );
    };
}