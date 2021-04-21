#include "engine/components/fanFxTransform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "core/memory/fanSerializable.hpp"
#include "core/math/fanMathUtils.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void FxTransform::SetInfo( EcsComponentInfo& _info )
    {
        _info.save = &FxTransform::Save;
        _info.load = &FxTransform::Load;
    }

    //========================================================================================================
    //========================================================================================================
    void FxTransform::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        FxTransform& transform = static_cast<FxTransform&>( _component );
        transform.mRotation = Quaternion::sIdentity;
        transform.mPosition = Vector3::sZero;
        transform.mScale    = Vector3::sOne;
    }

    //========================================================================================================
    //========================================================================================================
    void FxTransform::Save( const EcsComponent& _component, Json& _json )
    {
        const FxTransform& transform = static_cast<const FxTransform&>( _component );

        Serializable::SaveVec3( _json, "position", transform.mPosition );
        Serializable::SaveQuat( _json, "rotation", transform.mRotation );
        Serializable::SaveVec3( _json, "scale", transform.mScale );
    }

    //========================================================================================================
    //========================================================================================================
    void FxTransform::Load( EcsComponent& _component, const Json& _json )
    {
        FxTransform& transform = static_cast<FxTransform&>( _component );
        Serializable::LoadVec3( _json, "position", transform.mPosition );
        Serializable::LoadQuat( _json, "rotation", transform.mRotation );
        Serializable::LoadVec3( _json, "scale", transform.mScale );
    }

    //========================================================================================================
    //========================================================================================================
    glm::mat4 FxTransform::GetModelMatrix() const
    {
        glm::vec3 position = Math::ToGLM( mPosition );
        glm::vec3 glmScale = Math::ToGLM( mScale );
        glm::quat rotation = Math::ToGLM( mRotation );

        return glm::translate( glm::mat4( 1.f ), position ) *
               glm::mat4_cast( rotation ) *
               glm::scale( glm::mat4( 1.f ), glmScale );
    }

    //========================================================================================================
    //========================================================================================================
    glm::mat4 FxTransform::GetNormalMatrix() const
    {
        return glm::transpose( glm::inverse( GetModelMatrix() ) );
    }

    //========================================================================================================
    //========================================================================================================
    void FxTransform::LookAt( const Vector3& _point, const Vector3& _up )
    {
        const Vector3 forward = ( _point - mPosition ).Normalized();
        mRotation = Quaternion::LookRotation( forward, _up );
    }

    //========================================================================================================
    //========================================================================================================
    Vector3 FxTransform::TransformPoint( const Vector3 _point ) const
    {
        return mRotation * (/*mScale **/ _point ) + mPosition;
    }

    //========================================================================================================
    //========================================================================================================
    Vector3 FxTransform::InverseTransformPoint( const Vector3 _point ) const
    {
       /* const Vector3 invertScale( Fixed( 1 ) / mScale.x, Fixed( 1 ) / mScale.y, Fixed( 1 ) / mScale.z );*/
        return /*invertScale **/ ( mRotation.Inverse() * ( _point - mPosition ) );
    }

    //========================================================================================================
    // No translation applied
    //========================================================================================================
    Vector3 FxTransform::TransformDirection( const Vector3 _point ) const
    {
        return mRotation * _point;
    }

    //========================================================================================================
    // No translation applied
    //========================================================================================================*
    Vector3 FxTransform::InverseTransformDirection( const Vector3 _point ) const
    {
        return mRotation.Inverse() * _point;
    }
}