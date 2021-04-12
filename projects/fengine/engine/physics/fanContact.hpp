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
        FxRigidbody* rb0;
        FxRigidbody* rb1;
        FxTransform* transform0;
        FxTransform* transform1;
        Vector3 relativeContactPosition0;
        Vector3 relativeContactPosition1;
        Vector3 tangents[2];
        Vector3 normal;
        Vector3 position;
        Fixed   restitution;
        Fixed   penetration;
        Matrix3 contactToWorld;

        bool operator<( const Contact& _contact ) const
        {
            return penetration < _contact.penetration;
        }
    };

}