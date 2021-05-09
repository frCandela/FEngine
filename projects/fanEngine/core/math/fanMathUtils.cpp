#include "core/math/fanMathUtils.hpp"

#include "core/math/fanQuaternion.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    glm::vec3 Math::ToGLM( const Vector3& _vec )
    {
        return glm::vec3( _vec.x.ToFloat(), _vec.y.ToFloat(), _vec.z.ToFloat() );
    }

    //========================================================================================================
    //========================================================================================================
    glm::quat Math::ToGLM( const Quaternion& _quat )
    {
        glm::quat rotation;
        rotation.x = _quat.mAxis.x.ToFloat();
        rotation.y = _quat.mAxis.y.ToFloat();
        rotation.z = _quat.mAxis.z.ToFloat();
        rotation.w = _quat.mAngle.ToFloat();
        return rotation;
    }

    //========================================================================================================
    //========================================================================================================
    Vector3 Math::ToFixed( const glm::vec3& _vec )
    {
        return Vector3( Fixed::FromFloat( _vec.x ), Fixed::FromFloat( _vec.y ), Fixed::FromFloat( _vec.z ) );
    }

    //========================================================================================================
    //========================================================================================================
    Quaternion Math::ToFixed( const glm::quat& _quat )
    {
        Quaternion rotation;
        rotation.mAxis.x = Fixed::FromFloat( _quat.x );
        rotation.mAxis.y = Fixed::FromFloat( _quat.y );
        rotation.mAxis.z = Fixed::FromFloat( _quat.z );
        rotation.mAngle  = Fixed::FromFloat( _quat.w );
        return rotation;
    }
}