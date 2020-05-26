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
	//================================================================================================================================
	//================================================================================================================================
	void EditorCamera::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::CAMERA16;
		_info.onGui = &EditorCamera::OnGui;
		_info.name = "editor camera";
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorCamera::Init( EcsWorld& _world, EcsSingleton& _component )
	{
		EditorCamera& editorCamera = static_cast<EditorCamera&>( _component );

		editorCamera.cameraHandle = 0;
		editorCamera.speed = 10.f;
		editorCamera.speedMultiplier = 3.f;
		editorCamera.xySensitivity = btVector2( 0.005f, 0.005f );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorCamera::OnGui( EcsWorld&, EcsSingleton& _component )
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
	void EditorCamera::Update( EcsWorld& _world, const float _delta )
	{
		EditorCamera& editorCamera = _world.GetSingleton<EditorCamera>();
		const EcsEntity cameraEntity = _world.GetEntity( editorCamera.cameraHandle );
		Transform& cameraTransform = _world.GetComponent<Transform>( cameraEntity );
		Camera&	cameraCamera = _world.GetComponent<Camera>( cameraEntity );


		if( Mouse::Get().GetButtonPressed( Mouse::button1 ) )
		{
			Mouse::Get().LockCursor( true );
		}
		if( Mouse::Get().GetButtonDown( Mouse::button1, true ) == false )
		{
			Mouse::Get().LockCursor( false );
		}

		btVector3 position = cameraTransform.GetPosition();

		float forwardAxis	= Input::Get().Manager().GetAxis( "editor_forward" );
		float upAxis		= Input::Get().Manager().GetAxis( "editor_up" );
		float leftAxis		= Input::Get().Manager().GetAxis( "editor_left" );
		float boost			= Input::Get().Manager().GetAxis( "editor_boost" );

		// Calculates speed
		float realSpeed = editorCamera.speed;
		if( boost > 0.f )
		{
			realSpeed *= editorCamera.speedMultiplier;
		}

		position += _delta * realSpeed * leftAxis * cameraTransform.Left();		// Camera goes left		
		position += _delta * realSpeed * upAxis * cameraTransform.Up();			// Camera goes up		
		position += _delta * realSpeed * forwardAxis * cameraTransform.Forward();	// Camera goes forward

		// Camera rotation
		const btVector2 mouseDelta = Mouse::Get().GetDelta();
		const btVector2 mousePos = Mouse::Get().GetPosition();
		if( Mouse::Get().GetButtonDown( Mouse::button1 ) )
		{
			// Rotation depending on mouse movement
			const float invertAxis = -1;
			const btQuaternion rotationY( btVector3::Up(), -editorCamera.xySensitivity.x() * mouseDelta.x() );
			const btQuaternion rotationX( invertAxis * cameraTransform.Left(), -editorCamera.xySensitivity.y() * mouseDelta.y() );
			cameraTransform.SetRotationQuat( rotationX * rotationY * cameraTransform.GetRotationQuat() );

			// Remove roll
			const btVector3 relativeLeft = cameraTransform.Left();
			const btVector3 leftNoRoll( relativeLeft.x(), 0, relativeLeft.z() );
			const btVector3 axis = relativeLeft.cross( leftNoRoll );
			const float angle = leftNoRoll.angle( relativeLeft );
			if( angle != 0 )
			{

				const btQuaternion rot( axis, angle );

				cameraTransform.SetRotationQuat( rot * cameraTransform.GetRotationQuat() );
			}
		}
		cameraTransform.SetPosition( position );

		const Ray ray = cameraCamera.ScreenPosToRay( cameraTransform, btVector2( 0.9f, 0.9f ) );
		const float size = 0.002f;
		btVector3 offset = ray.origin + 0.1f * ray.direction;
	}

	//================================================================================================================================
	// creates the editor camera entity & components
	// setups the EditorCamera singleton
	//================================================================================================================================
	void EditorCamera::CreateEditorCamera( EcsWorld& _world )
	{
		Scene& scene = _world.GetSingleton< Scene >();

		// Editor Camera
		SceneNode& cameraNode = scene.CreateSceneNode( "editor_camera", &scene.GetRootNode() );
		EcsEntity cameraID = _world.GetEntity( cameraNode.handle );
		cameraNode.AddFlag( SceneNode::NOT_SAVED | SceneNode::NO_DELETE | SceneNode::NO_RAYCAST );

		Transform& transform = _world.AddComponent< Transform >( cameraID );
		Camera& camera = _world.AddComponent< Camera >( cameraID );

		transform.SetPosition( btVector3( 0, 0, -2 ) );
		scene.mainCameraHandle = cameraNode.handle;

		// set editor camera singleton
		EditorCamera& editorCamera = _world.GetSingleton<EditorCamera>();
		editorCamera.cameraHandle = cameraNode.handle;
	}
}