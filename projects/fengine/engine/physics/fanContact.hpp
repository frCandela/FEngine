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
        FxTransform* transform[2] = { nullptr, nullptr };
        Vector3 relativeContactPosition[2];
        Vector3 normal;
        Vector3 position;
        Fixed   restitution;
        Fixed   penetration;
        Matrix3 contactToWorld;
    };
}