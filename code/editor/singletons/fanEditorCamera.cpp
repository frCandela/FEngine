#include "editor/singletons/fanEditorCamera.hpp"

#include "core/input/fanInput.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/shapes/fanRay.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/singletons/fanMouse.hpp"
#include "scene/singletons/fanScene.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EditorCamera::SetInfo( EcsSingletonInfo& /*_info*/ )
	{
	}

	//========================================================================================================
	//========================================================================================================
	void EditorCamera::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		EditorCamera& editorCamera = static_cast<EditorCamera&>( _component );

		editorCamera.mCameraHandle    = 0;
		editorCamera.mSpeed           = 10.f;
		editorCamera.mSpeedMultiplier = 3.f;
		editorCamera.mXYSensitivity   = btVector2( 0.005f, 0.005f );
	}

	//========================================================================================================
	// updates the editor camera position & rotation
	//========================================================================================================
	void EditorCamera::Update( EcsWorld& _world, const float _delta )
	{
        Mouse& mouse = _world.GetSingleton<Mouse>();
        if( !mouse.mWindowHovered )             { return; }
        if( mouse.mPressed[Mouse::button2] )    { mouse.sLocked = true; }
        if( !mouse.mDown[Mouse::button2] )      { mouse.sLocked = false; }
        if( !mouse.sLocked )                    { return; }

        EditorCamera& editorCamera = _world.GetSingleton<EditorCamera>();
		const EcsEntity cameraEntity = _world.GetEntity( editorCamera.mCameraHandle );
		Transform& cameraTransform = _world.GetComponent<Transform>( cameraEntity );
		Camera&	cameraCamera = _world.GetComponent<Camera>( cameraEntity );

		btVector3 position = cameraTransform.GetPosition();
		float forwardAxis	= Input::Get().Manager().GetAxis( "editor_forward" );
		float upAxis		= Input::Get().Manager().GetAxis( "editor_up" );
		float leftAxis		= Input::Get().Manager().GetAxis( "editor_left" );
		float boost			= Input::Get().Manager().GetAxis( "editor_boost" );

		// Calculates speed
		float realSpeed = editorCamera.mSpeed;
		if( boost > 0.f )
		{
			realSpeed *= editorCamera.mSpeedMultiplier;
		}

		position += _delta * realSpeed * leftAxis * cameraTransform.Left();		// Camera goes left		
		position += _delta * realSpeed * upAxis * cameraTransform.Up();			// Camera goes up		
		position += _delta * realSpeed * forwardAxis * cameraTransform.Forward();	// Camera goes forward

		// Camera rotation
		const glm::vec2 mouseDelta = mouse.mPositionDelta;
		const glm::vec2 mousePos = mouse.mLocalPosition;
		if( mouse.mDown[ Mouse::button2 ] )
		{
			// Rotation depending on mouse movement
			const float invertAxis = -1;
			const btQuaternion rotationY( btVector3::Up(), -editorCamera.mXYSensitivity.x() * mouseDelta.x );
            const btQuaternion rotationX( invertAxis * cameraTransform.Left(),
                                          -editorCamera.mXYSensitivity.y() * mouseDelta.y );
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

	//========================================================================================================
	// creates the editor camera entity & components
	// setups the EditorCamera singleton
	//========================================================================================================
	void EditorCamera::CreateEditorCamera( EcsWorld& _world )
	{
		Scene& scene = _world.GetSingleton< Scene >();

		// Editor Camera
		SceneNode& cameraNode = scene.CreateSceneNode( "editor_camera", &scene.GetRootNode() );
		EcsEntity cameraID = _world.GetEntity( cameraNode.mHandle );
		cameraNode.AddFlag( SceneNode::NoSave | SceneNode::NoDelete | SceneNode::NoRaycast );

		Transform& transform = _world.AddComponent< Transform >( cameraID );
		_world.AddComponent< Camera >( cameraID );

		transform.SetPosition( btVector3( 0, 0, -2 ) );
		scene.mMainCameraHandle = cameraNode.mHandle;

		// set editor camera singleton
		EditorCamera& editorCamera = _world.GetSingleton<EditorCamera>();
		editorCamera.mCameraHandle = cameraNode.mHandle;
	}
}