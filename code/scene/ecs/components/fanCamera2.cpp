#include "scene/ecs/components/fanCamera2.hpp"

#include "scene/ecs/components/fanTransform2.hpp"
#include "render/fanRenderSerializable.hpp"
#include "core/math/shapes/fanRay.hpp"

namespace fan
{
	REGISTER_COMPONENT( Camera2, "camera" );

	//================================================================================================================================
	//================================================================================================================================
	void Camera2::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::CAMERA16;
		_info.onGui = &Camera2::OnGui;
		_info.clear = &Camera2::Clear;
		_info.load = &Camera2::Load;
		_info.save = &Camera2::Save;
		_info.editorPath = "";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Camera2::Clear( ecComponent& _camera )
	{
		Camera2& camera = static_cast<Camera2&>( _camera );

		camera.fov = 110.f;
		camera.aspectRatio = 1.f;
		camera.nearDistance = 0.01f;
		camera.farDistance = 1000.f;
		camera.orthoSize = 10.f;
		camera.type = Camera2::PERSPECTIVE;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Camera2::OnGui( ecComponent& _camera )
	{
		Camera2& camera = static_cast<Camera2&>( _camera );

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
	void Camera2::Save( const ecComponent& _camera, Json& _json )
	{
		const Camera2& camera = static_cast<const Camera2&>( _camera );

		Serializable::SaveInt( _json, "camera_type", camera.type );
		Serializable::SaveFloat( _json, "orthoSize", camera.orthoSize );
		Serializable::SaveFloat( _json, "fov", camera.fov );
		Serializable::SaveFloat( _json, "nearDistance", camera.nearDistance );
		Serializable::SaveFloat( _json, "farDistance", camera.farDistance );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Camera2::Load( ecComponent& _camera, const Json& _json )
	{
		Camera2& camera = static_cast<Camera2&>( _camera );

		Serializable::LoadInt(   _json, "camera_type", (int&)camera.type );
		Serializable::LoadFloat( _json, "orthoSize",	camera.orthoSize );
		Serializable::LoadFloat( _json, "fov",			camera.fov );
		Serializable::LoadFloat( _json, "nearDistance", camera.nearDistance );
		Serializable::LoadFloat( _json, "farDistance",	camera.farDistance );
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::mat4 Camera2::GetView( const Transform2& _cameraTransform ) const
	{
		glm::mat4 view = glm::lookAt(
			ToGLM( _cameraTransform.GetPosition() ),
			ToGLM( _cameraTransform.GetPosition() + _cameraTransform.Forward() ),
			ToGLM( _cameraTransform.Up() ) );
		return view;
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::mat4 Camera2::GetProjection() const
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
	Ray Camera2::ScreenPosToRay( const Transform2& _cameraTransform, const btVector2& _screenSpacePosition )
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
			ray.direction = ( 100.f * ( ray.origin - pos ) ).normalized();

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
	btVector2 Camera2::WorldPosToScreen( const Transform2& _cameraTransform, const btVector3& worldPosition )
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