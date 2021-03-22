#include "engine/components/fanCamera.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "core/math/fanMathUtils.hpp"
#include "engine/components/fanTransform.hpp"
#include "render/fanRenderSerializable.hpp"
#include "core/shapes/fanRay.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Camera::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &Camera::Load;
		_info.save        = &Camera::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void Camera::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		Camera& camera = static_cast<Camera&>( _component );

		camera.mFov          = 110.f;
		camera.mAspectRatio  = 1.f;
		camera.mNearDistance = 0.01f;
		camera.mFarDistance  = 1000.f;
		camera.mOrthoSize    = 10.f;
		camera.mType         = Camera::Perspective;
	}

	//========================================================================================================
	//========================================================================================================
	void Camera::Save( const EcsComponent& _component, Json& _json )
	{
		const Camera& camera = static_cast<const Camera&>( _component );

		Serializable::SaveInt( _json, "camera_type", camera.mType );
		Serializable::SaveFloat( _json, "orthoSize", camera.mOrthoSize );
		Serializable::SaveFloat( _json, "fov", camera.mFov );
		Serializable::SaveFloat( _json, "nearDistance", camera.mNearDistance );
		Serializable::SaveFloat( _json, "farDistance", camera.mFarDistance );
	}

	//========================================================================================================
	//========================================================================================================
	void Camera::Load( EcsComponent& _component, const Json& _json )
	{
		Camera& camera = static_cast<Camera&>( _component );

		int cameraType;
		Serializable::LoadInt(   _json, "camera_type", cameraType );
        camera.mType = Type(cameraType);

		Serializable::LoadFloat( _json, "orthoSize",	camera.mOrthoSize );
		Serializable::LoadFloat( _json, "fov",			camera.mFov );
		Serializable::LoadFloat( _json, "nearDistance", camera.mNearDistance );
		Serializable::LoadFloat( _json, "farDistance",	camera.mFarDistance );
	}

	//========================================================================================================
	//========================================================================================================
	glm::mat4 Camera::GetView( const Transform& _cameraTransform ) const
	{
		glm::mat4 view = glm::lookAt(
			ToGLM( _cameraTransform.GetPosition() ),
			ToGLM( _cameraTransform.GetPosition() + _cameraTransform.Forward() ),
			ToGLM( _cameraTransform.Up() ) );
		return view;
	}

	//========================================================================================================
	//========================================================================================================
	glm::mat4 Camera::GetProjection() const
	{
		glm::mat4 proj = glm::mat4( 1 );
		if( mType == Type::Orthogonal )
		{
            proj = glm::ortho( -mOrthoSize * mAspectRatio,
                               mOrthoSize * mAspectRatio,
                               -mOrthoSize,
                               mOrthoSize,
                               mNearDistance,
                               mFarDistance );
		}
		else if( mType == Type::Perspective )
		{
			const float fov_rad = glm::radians( mFov );
			proj = glm::perspective( fov_rad, mAspectRatio, mNearDistance, mFarDistance );
		}
		return proj;
	}

	//========================================================================================================
	// Returns a ray going from camera through a screen point
	// ( with screenSpacePosition between {-1.f,-1.f} and {1.f,1.f} ).
	//========================================================================================================
	Ray Camera::ScreenPosToRay( const Transform& _cameraTransform,
	                            const btVector2& _screenSpacePosition ) const
	{
        fanAssert( _screenSpacePosition.x() >= -1.f && _screenSpacePosition.x() <= 1.f );
        fanAssert( _screenSpacePosition.y() >= -1.f && _screenSpacePosition.y() <= 1.f );

		if( mType == Type::Perspective )
		{
			const btVector3	pos		= _cameraTransform.GetPosition();
			const btVector3 upVec	= _cameraTransform.Up();
			const btVector3 left	= _cameraTransform.Left();
			const btVector3 forward = _cameraTransform.Forward();

			btVector3 nearMiddle = pos + mNearDistance * forward;

			float nearHeight = mNearDistance * tan( glm::radians( mFov / 2 ) );
			float nearWidth = mAspectRatio * nearHeight;

			Ray ray;
			ray.origin = nearMiddle - _screenSpacePosition.x() * nearWidth * left - _screenSpacePosition.y() * nearHeight * upVec;
			ray.direction = 100.f * ( ray.origin - pos ) ;
			if( !ray.direction.fuzzyZero() ) { ray.direction.normalize(); }


			return ray;
		}
		else
		{ // ORTHOGONAL
			Ray ray;

			ray.origin = _cameraTransform.GetPosition();
			ray.origin -= mAspectRatio * mOrthoSize * _cameraTransform.Left() * _screenSpacePosition[0];
			ray.origin -= mOrthoSize * _cameraTransform.Up() * _screenSpacePosition[1];

			ray.direction = _cameraTransform.Forward();

			return ray;
		}
	}

	//========================================================================================================
	//========================================================================================================
	btVector2 Camera::WorldPosToScreen( const Transform& _cameraTransform,
	                                    const btVector3& worldPosition ) const
	{
		if( mType == Type::Perspective )
		{
			const glm::vec4 pos( worldPosition[0], worldPosition[1], worldPosition[2], 1.f );
			glm::vec4  proj = GetProjection() * GetView( _cameraTransform ) * pos;
			proj /= proj.z;
			return btVector2( proj.x, proj.y );
		}
		else
		{
			const glm::vec4 pos( worldPosition[0], worldPosition[1], worldPosition[2], 1.f );
			glm::vec4  proj = GetProjection() * GetView( _cameraTransform ) * pos;
			return btVector2( proj.x, -proj.y );
		}
	}
}