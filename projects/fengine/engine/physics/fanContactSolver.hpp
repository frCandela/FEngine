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

        Fixed mRestingVelocityLimit = FIXED( 0.15 );
        Fixed mAngularLimitNonLinearProjection = FIXED( 0.01 );
        Fixed mRestitution                     = FIXED( 0.5); // [0,1]

        void ResolveContacts( std::vector<Contact> _contacts, const Fixed _deltaTime );
        void PrepareContacts( std::vector<Contact>& _contacts, const Fixed _deltaTime );
        void ResolvePositions( std::vector<Contact>& _contacts );
        void ResolveVelocities( std::vector<Contact>& _contacts, const Fixed _deltaTime );

        /*static*/ void ResolveVelocity( const Contact& _contact, const Fixed _deltaTime );
        /*static*/ void ResolvePosition( const Contact& _contact, const Fixed _angularLimitNonLinearProjection, Vector3* _outRotationChange, Vector3* _outVelocityChange );

        static Vector3 CalculateRelativeVelocity( const Contact& _contact );
        static Fixed CalculateDesiredTotalDeltaVelocity( const Contact& _contact, const Fixed _deltaTime, const Fixed _restingVelocityLimit );
        static Fixed CalculateDeltaVelocityPerUnitImpulse( const Contact& _contact, const int _index );
    };
}