#pragma once

#include "core/math/fanVector3.hpp"
#include "core/math/fanMatrix3.hpp"

namespace fan
{
    struct FxRigidbody;
    struct FxTransform;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Contact
    {
        FxRigidbody* rigidbody[2] = { nullptr, nullptr };
        Vector3 normal;
        Vector3 position;
        Fixed   penetration;

        // these fields are filled by ContactSolver::PrepareContacts
        Vector3 relativeContactPosition[2];
        Matrix3 contactToWorld;
        Vector3 relativeVelocity; // in contact coordinates
        Fixed   restitution;
        Fixed   desiredTotalDeltaVelocity;
        Fixed   totalInverseMass;
    };
}