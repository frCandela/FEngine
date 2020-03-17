#include "editor/singletonComponents/fanEditorCamera.hpp"

#include "core/input/fanMouse.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/math/shapes/fanRay.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"


namespace fan
{
	REGISTER_SINGLETON_COMPONENT( EditorCamera, "editor_camera" );

	//================================================================================================================================
	//================================================================================================================================
	void EditorCamera::Update( EditorCamera& _camera, const float _delta )
	{
		if( Mouse::Get().GetButtonPressed( Mouse::button1 ) )
		{
			Mouse::Get().LockCursor( true );
		}
		if( Mouse::Get().GetButtonDown( Mouse::button1, true ) == false )
		{
			Mouse::Get().LockCursor( false );
		}

		btVector3 position = _camera.transform->GetPosition();

		float forwardAxis	= Input::Get().Manager().GetAxis( "editor_forward" );
		float upAxis		= Input::Get().Manager().GetAxis( "editor_up" );
		float leftAxis		= Input::Get().Manager().GetAxis( "editor_left" );
		float boost			= Input::Get().Manager().GetAxis( "editor_boost" );

		// Calculates speed
		float realSpeed = _camera.speed;
		if( boost > 0.f )
		{
			realSpeed *= _camera.speedMultiplier;
		}

		position += _delta * realSpeed * leftAxis * _camera.transform->Left();		// Camera goes left		
		position += _delta * realSpeed * upAxis * _camera.transform->Up();			// Camera goes up		
		position += _delta * realSpeed * forwardAxis * _camera.transform->Forward();	// Camera goes forward

		// Camera rotation
		const btVector2 mouseDelta = Mouse::Get().GetDelta();
		const btVector2 mousePos = Mouse::Get().GetPosition();
		if( Mouse::Get().GetButtonDown( Mouse::button1 ) )
		{
			// Rotation depending on mouse movement
			const float invertAxis = -1;
			const btQuaternion rotationY( btVector3::Up(), -_camera.xySensitivity.x() * mouseDelta.x() );
			const btQuaternion rotationX( invertAxis * _camera.transform->Left(), -_camera.xySensitivity.y() * mouseDelta.y() );
			_camera.transform->SetRotationQuat( rotationX * rotationY * _camera.transform->GetRotationQuat() );

			// Remove roll
			const btVector3 relativeLeft = _camera.transform->Left();
			const btVector3 leftNoRoll( relativeLeft.x(), 0, relativeLeft.z() );
			const btVector3 axis = relativeLeft.cross( leftNoRoll );
			const float angle = leftNoRoll.angle( relativeLeft );
			if( angle != 0 )
			{

				const btQuaternion rot( axis, angle );

				_camera.transform->SetRotationQuat( rot * _camera.transform->GetRotationQuat() );
			}
		}
		_camera.transform->SetPosition( position );

		const Ray ray = _camera.camera->ScreenPosToRay( *_camera.transform, btVector2( 0.9f, 0.9f ) );
		const float size = 0.002f;
		btVector3 offset = ray.origin + 0.1f * ray.direction;
	}
}