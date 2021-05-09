#include "fanFxTransform.hpp"
#include "core/memory/fanSerializable.hpp"
#include "core/math/fanMathUtils.hpp"

namespace fan
{
    const FxTransform FxTransform::sIdentity = FxTransform::Make( Quaternion::sIdentity, Vector3::sZero );

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FxTransform::SetInfo( EcsComponentInfo& _info )
    {
        _info.save = &FxTransform::Save;
        _info.load = &FxTransform::Load;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FxTransform::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        FxTransform& transform = static_cast<FxTransform&>( _component );
        transform.mRotation = Quaternion::sIdentity;
        transform.mPosition = Vector3::sZero;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FxTransform::Save( const EcsComponent& _component, Json& _json )
    {
        const FxTransform& transform = static_cast<const FxTransform&>( _component );

        Serializable::SaveVec3( _json, "position", transform.mPosition );
        Serializable::SaveQuat( _json, "rotation", transform.mRotation );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FxTransform::Load( EcsComponent& _component, const Json& _json )
    {
        FxTransform& transform = static_cast<FxTransform&>( _component );
        Serializable::LoadVec3( _json, "position", transform.mPosition );
        Serializable::LoadQuat( _json, "rotation", transform.mRotation );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    FxTransform FxTransform::Make( const Quaternion& _rotation, const Vector3& _position )
    {
        FxTransform transform;
        transform.mRotation = _rotation;
        transform.mPosition = _position;
        return transform;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    glm::mat4 FxTransform::GetModelMatrix( const Vector3& _scale ) const
    {
        glm::vec3 position = Math::ToGLM( mPosition );
        glm::vec3 glmScale = Math::ToGLM( _scale );
        glm::quat rotation = Math::ToGLM( mRotation );

        return glm::translate( glm::mat4( 1.f ), position ) *
               glm::mat4_cast( rotation ) *
               glm::scale( glm::mat4( 1.f ), glmScale );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    glm::mat4 FxTransform::GetNormalMatrix( const Vector3& _scale ) const
    {
        return glm::transpose( glm::inverse( GetModelMatrix( _scale ) ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FxTransform::LookAt( const Vector3& _point, const Vector3& _up )
    {
        const Vector3 forward = ( _point - mPosition ).Normalized();
        mRotation = Quaternion::LookRotation( forward, _up );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    FxTransform FxTransform::Inverse() const
    {
        FxTransform inverse;
        inverse.mRotation = mRotation.Inverse();
        inverse.mPosition = -( inverse.mRotation * mPosition );
        return inverse;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Vector3 FxTransform::TransformPoint( const Vector3 _point ) const
    {
        return mRotation * _point + mPosition;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Vector3 FxTransform::InverseTransformPoint( const Vector3 _point ) const
    {
        return mRotation.Inverse() * ( _point - mPosition );
    }

    //==================================================================================================================================================================================================
    // No translation applied
    //==================================================================================================================================================================================================
    Vector3 FxTransform::TransformDirection( const Vector3 _point ) const
    {
        return mRotation * _point;
    }

    //==================================================================================================================================================================================================
    // No translation applied
    //========================================================================================================*
    Vector3 FxTransform::InverseTransformDirection( const Vector3 _point ) const
    {
        return mRotation.Inverse() * _point;
    }
}