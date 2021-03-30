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
        rotation.x = _quat.v.x.ToFloat();
        rotation.y = _quat.v.y.ToFloat();
        rotation.z = _quat.v.z.ToFloat();
        rotation.w = _quat.n.ToFloat();
        return rotation;
    }
}