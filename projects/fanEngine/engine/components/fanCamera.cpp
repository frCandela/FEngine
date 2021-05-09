#include "engine/components/fanCamera.hpp"

#include "core/math/fanMathUtils.hpp"
#include "engine/physics/fanFxTransform.hpp"
#include "render/fanRenderSerializable.hpp"
#include "core/shapes/fanRay.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Camera::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &Camera::Load;
        _info.save = &Camera::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Camera::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        Camera& camera = static_cast<Camera&>( _component );

        camera.mFov          = 90;
        camera.mAspectRatio  = 1;
        camera.mNearDistance = FIXED( 0.01 );
        camera.mFarDistance  = 1000;
        camera.mOrthoSize    = 10;
        camera.mType         = Camera::Perspective;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Camera::Save( const EcsComponent& _component, Json& _json )
    {
        const Camera& camera = static_cast<const Camera&>( _component );

        Serializable::SaveInt( _json, "camera_type", camera.mType );
        Serializable::SaveFixed( _json, "orthoSize", camera.mOrthoSize );
        Serializable::SaveFixed( _json, "fov", camera.mFov );
        Serializable::SaveFixed( _json, "nearDistance", camera.mNearDistance );
        Serializable::SaveFixed( _json, "farDistance", camera.mFarDistance );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Camera::Load( EcsComponent& _component, const Json& _json )
    {
        Camera& camera = static_cast<Camera&>( _component );

        int cameraType;
        Serializable::LoadInt( _json, "camera_type", cameraType );
        camera.mType = Type( cameraType );

        Serializable::LoadFixed( _json, "orthoSize", camera.mOrthoSize );
        Serializable::LoadFixed( _json, "fov", camera.mFov );
        Serializable::LoadFixed( _json, "nearDistance", camera.mNearDistance );
        Serializable::LoadFixed( _json, "farDistance", camera.mFarDistance );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    glm::mat4 Camera::GetView( const FxTransform& _cameraTransform ) const
    {
        return glm::lookAt( Math::ToGLM( _cameraTransform.mPosition ),
                            Math::ToGLM( _cameraTransform.mPosition + _cameraTransform.Forward() ),
                            Math::ToGLM( _cameraTransform.Up() ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    glm::mat4 Camera::GetProjection() const
    {
        glm::mat4 proj = glm::mat4( 1 );
        if( mType == Type::Orthogonal )
        {
            proj = glm::ortho( -( mOrthoSize * mAspectRatio ).ToFloat(),
                               ( mOrthoSize * mAspectRatio ).ToFloat(),
                               -mOrthoSize.ToFloat(),
                               mOrthoSize.ToFloat(),
                               mNearDistance.ToFloat(),
                               mFarDistance.ToFloat() );
        }
        else if( mType == Type::Perspective )
        {
            const float fov_rad = glm::radians( mFov.ToFloat() );
            proj = glm::perspective( fov_rad, mAspectRatio.ToFloat(), mNearDistance.ToFloat(), mFarDistance.ToFloat() );
        }
        return proj;
    }

    //==================================================================================================================================================================================================
    // Returns a ray going from camera through a screen point
    // ( with screenSpacePosition between {-1.f,-1.f} and {1.f,1.f} ).
    //==================================================================================================================================================================================================
    Ray Camera::ScreenPosToRay( const FxTransform& _cameraTransform, const glm::vec2& _screenSpacePosition ) const
    {
        fanAssert( _screenSpacePosition.x >= -1.f && _screenSpacePosition.x <= 1.f );
        fanAssert( _screenSpacePosition.y >= -1.f && _screenSpacePosition.y <= 1.f );

        if( mType == Type::Perspective )
        {
            const Vector3 pos     = _cameraTransform.mPosition;
            const Vector3 upVec   = _cameraTransform.Up();
            const Vector3 left    = _cameraTransform.Left();
            const Vector3 forward = _cameraTransform.Forward();

            Vector3 nearMiddle = pos + mNearDistance * forward;

            Fixed nearHeight = mNearDistance * Fixed::Tan( Fixed::Radians( mFov / 2 ) );
            Fixed nearWidth  = mAspectRatio * nearHeight;

            Ray ray;
            ray.origin    = nearMiddle - Fixed::FromFloat( _screenSpacePosition.x ) * nearWidth * left - Fixed::FromFloat( _screenSpacePosition.y ) * nearHeight * upVec;
            ray.direction = 100 * ( ray.origin - pos );
            if( !Vector3::IsFuzzyZero( ray.direction ) ){ ray.direction.Normalize(); }

            return ray;
        }
        else
        { // ORTHOGONAL
            Ray ray;

            ray.origin = _cameraTransform.mPosition;
            ray.origin -= mAspectRatio * mOrthoSize * _cameraTransform.Left() * Fixed::FromFloat( _screenSpacePosition[0] );
            ray.origin -= mOrthoSize * _cameraTransform.Up() * Fixed::FromFloat( _screenSpacePosition[1] );

            ray.direction = _cameraTransform.Forward();

            return ray;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    glm::vec2 Camera::WorldPosToScreen( const FxTransform& _cameraTransform, const Vector3& _worldPosition ) const
    {
        if( mType == Type::Perspective )
        {
            const glm::vec4 pos( Math::ToGLM( _worldPosition ), 1.f );
            glm::vec4       proj = GetProjection() * GetView( _cameraTransform ) * pos;
            proj /= proj.z;
            return glm::vec2( proj.x, proj.y );
        }
        else
        {
            const glm::vec4 pos( Math::ToGLM( _worldPosition ), 1.f );
            glm::vec4       proj = GetProjection() * GetView( _cameraTransform ) * pos;
            return glm::vec2( proj.x, -proj.y );
        }
    }
}