#include "editor/singletonComponents/fanEditorCamera.hpp"

#include "core/input/fanMouse.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/math/shapes/fanRay.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( EditorCamera );

	//================================================================================================================================
	//================================================================================================================================
	void EditorCamera::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::CAMERA16;
		_info.init = &EditorCamera::Init;
		_info.onGui = &EditorCamera::OnGui;
		_info.name = "editor camera";
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorCamera::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		EditorCamera& editorCamera = static_cast<EditorCamera&>( _component );

		editorCamera.cameraNode = nullptr;
		editorCamera.transform = nullptr;
		editorCamera.camera = nullptr;
		editorCamera.speed = 10.f;
		editorCamera.speedMultiplier = 3.f;
		editorCamera.xySensitivity = btVector2( 0.005f, 0.005f );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorCamera::OnGui( SingletonComponent& _component )
	{
		EditorCamera& editorCamera = static_cast<EditorCamera&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::DragFloat( "speed", &editorCamera.speed, 1.f, 0.f, 10000.f );
			ImGui::DragFloat( "speed multiplier", &editorCamera.speedMultiplier, 1.f, 0.f, 10000.f );
			ImGui::DragFloat2( "xy sensitivity", &editorCamera.xySensitivity[0], 1.f, 0.f, 1.f );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}


	//================================================================================================================================
	// updates the editor camera position & rotation
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

	//================================================================================================================================
	// creates the editor camera entity & components
	// setups the EditorCamera singleton
	//================================================================================================================================
	void EditorCamera::CreateEditorCamera( EcsWorld& _world )
	{
		Scene& scene = _world.GetSingletonComponent< Scene >();

		// Editor Camera
		SceneNode& cameraNode = scene.CreateSceneNode( "editor_camera", scene.root );
		EntityID cameraID = _world.GetEntityID( cameraNode.handle );
		cameraNode.AddFlag( SceneNode::NOT_SAVED | SceneNode::NO_DELETE | SceneNode::NO_RAYCAST );

		Transform& transform = _world.AddComponent< Transform >( cameraID );
		Camera& camera = _world.AddComponent< Camera >( cameraID );

		transform.SetPosition( btVector3( 0, 0, -2 ) );
		scene.mainCamera = &cameraNode;

		// set editor camera singleton
		EditorCamera& editorCamera = _world.GetSingletonComponent<EditorCamera>();
		editorCamera.cameraNode = &cameraNode;
		editorCamera.transform = &transform;
		editorCamera.camera = &camera;
	}
}