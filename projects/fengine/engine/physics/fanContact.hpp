#pragma once

#include "core/math/fanVector3.hpp"

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
        Vector3 normal;
        Vector3 position;
        Fixed   restitution;
        Fixed   penetration;

        bool operator<( const Contact& _contact ) const
        {
            return penetration < _contact.penetration;
        }
    };

}