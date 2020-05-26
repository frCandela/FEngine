#include "scene/components/fanCamera.hpp"

#include "core/math/fanMathUtils.hpp"
#include "scene/components/fanTransform.hpp"
#include "render/fanRenderSerializable.hpp"
#include "core/math/shapes/fanRay.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void Camera::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::CAMERA16;
		_info.onGui = &Camera::OnGui;
		_info.load = &Camera::Load;
		_info.save = &Camera::Save;
		_info.editorPath = "/";
		_info.name = "camera";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Camera::Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
	{
		Camera& camera = static_cast<Camera&>( _component );

		camera.fov = 110.f;
		camera.aspectRatio = 1.f;
		camera.nearDistance = 0.01f;
		camera.farDistance = 1000.f;
		camera.orthoSize = 10.f;
		camera.type = Camera::PERSPECTIVE;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Camera::OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
	{
		Camera& camera = static_cast<Camera&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{

			int item = static_cast<int>( camera.type );
			if( ImGui::Combo( "type", &item, "perspective\0orthogonal\0" ) )
			{
				camera.type = Type( item );
			}

			if( camera.type == Type::PERSPECTIVE )
			{
				// fov
				if( ImGui::Button( "##fov" ) )
				{
					camera.fov = 110.f;
				}
				ImGui::SameLine();
				ImGui::DragFloat( "fov", &camera.fov, 1.f, 1.f, 179.f );

			}
			else if( camera.type == Type::ORTHOGONAL )
			{
				// fov
				if( ImGui::Button( "##size" ) )
				{
					camera.orthoSize = 10.f;
				}
				ImGui::SameLine();
				ImGui::DragFloat( "size", &camera.orthoSize, 1.f, 0.f, 100.f );

			}

			// nearDistance
			if( ImGui::Button( "##nearDistance" ) )
			{
				camera.nearDistance = 0.01f;
			}
			ImGui::SameLine();
			ImGui::DragFloat( "near distance", &camera.nearDistance, 0.001f, 0.01f, 10.f );


			// far distance
			if( ImGui::Button( "##fardistance" ) )
			{
				camera.farDistance = 1000.f;
			}
			ImGui::SameLine();
			ImGui::DragFloat( "far distance", &camera.farDistance, 10.f, 0.05f, 10000.f );

		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Camera::Save( const EcsComponent& _component, Json& _json )
	{
		const Camera& camera = static_cast<const Camera&>( _component );

		Serializable::SaveInt( _json, "camera_type", camera.type );
		Serializable::SaveFloat( _json, "orthoSize", camera.orthoSize );
		Serializable::SaveFloat( _json, "fov", camera.fov );
		Serializable::SaveFloat( _json, "nearDistance", camera.nearDistance );
		Serializable::SaveFloat( _json, "farDistance", camera.farDistance );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Camera::Load( EcsComponent& _component, const Json& _json )
	{
		Camera& camera = static_cast<Camera&>( _component );

		Serializable::LoadInt(   _json, "camera_type", (int&)camera.type );
		Serializable::LoadFloat( _json, "orthoSize",	camera.orthoSize );
		Serializable::LoadFloat( _json, "fov",			camera.fov );
		Serializable::LoadFloat( _json, "nearDistance", camera.nearDistance );
		Serializable::LoadFloat( _json, "farDistance",	camera.farDistance );
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::mat4 Camera::GetView( const Transform& _cameraTransform ) const
	{
		glm::mat4 view = glm::lookAt(
			ToGLM( _cameraTransform.GetPosition() ),
			ToGLM( _cameraTransform.GetPosition() + _cameraTransform.Forward() ),
			ToGLM( _cameraTransform.Up() ) );
		return view;
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::mat4 Camera::GetProjection() const
	{
		glm::mat4 proj = glm::mat4( 1 );
		if( type == Type::ORTHOGONAL )
		{
			proj = glm::ortho( -orthoSize * aspectRatio, orthoSize * aspectRatio, -orthoSize, orthoSize, nearDistance, farDistance );
		}
		else if( type == Type::PERSPECTIVE )
		{
			const float fov_rad = glm::radians( fov );
			proj = glm::perspective( fov_rad, aspectRatio, nearDistance, farDistance );
		}
		return proj;
	}

	//================================================================================================================================
	// Returns a ray going from camera through a screen point ( with screenSpacePosition between {-1.f,-1.f} and {1.f,1.f} ).
	//================================================================================================================================
	Ray Camera::ScreenPosToRay( const Transform& _cameraTransform, const btVector2& _screenSpacePosition ) const
	{
		assert( _screenSpacePosition.x() >= -1.f && _screenSpacePosition.x() <= 1.f );
		assert( _screenSpacePosition.y() >= -1.f && _screenSpacePosition.y() <= 1.f );

		if( type == Type::PERSPECTIVE )
		{
			const btVector3	pos		= _cameraTransform.GetPosition();
			const btVector3 upVec	= _cameraTransform.Up();
			const btVector3 left	= _cameraTransform.Left();
			const btVector3 forward = _cameraTransform.Forward();

			btVector3 nearMiddle = pos + nearDistance * forward;

			float nearHeight = nearDistance * tan( glm::radians( fov / 2 ) );
			float nearWidth = aspectRatio * nearHeight;

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
			ray.origin -= aspectRatio * orthoSize * _cameraTransform.Left() * _screenSpacePosition[0];
			ray.origin -= orthoSize * _cameraTransform.Up() * _screenSpacePosition[1];

			ray.direction = _cameraTransform.Forward();

			return ray;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector2 Camera::WorldPosToScreen( const Transform& _cameraTransform, const btVector3& worldPosition ) const
	{
		if( type == Type::PERSPECTIVE )
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