#include "editor/singletons/fanEditorCamera.hpp"

#include "core/input/fanInput.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/shapes/fanRay.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/components/fanCamera.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/singletons/fanMouse.hpp"
#include "engine/singletons/fanScene.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorCamera::SetInfo( EcsSingletonInfo& /*_info*/ )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorCamera::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
    {
        EditorCamera& editorCamera = static_cast<EditorCamera&>( _component );

        editorCamera.mCameraHandle    = 0;
        editorCamera.mSpeed           = 10;
        editorCamera.mSpeedMultiplier = 3;
        editorCamera.mXYSensitivity   = glm::vec2( 0.3f, 0.3f );
    }

    //==================================================================================================================================================================================================
    // updates the editor camera position & rotation
    //==================================================================================================================================================================================================
    void EditorCamera::Update( EcsWorld& _world, const Fixed _delta )
    {
        Mouse& mouse = _world.GetSingleton<Mouse>();
        if( !mouse.mWindowHovered ){ return; }
        if( mouse.mPressed[Mouse::button2] ){ mouse.sLocked = true; }
        if( !mouse.mDown[Mouse::button2] ){ mouse.sLocked = false; }
        if( !mouse.sLocked ){ return; }

        EditorCamera& editorCamera = _world.GetSingleton<EditorCamera>();
        const EcsEntity cameraEntity = _world.GetEntity( editorCamera.mCameraHandle );
        Transform& cameraTransform = _world.GetComponent<Transform>( cameraEntity );

        Vector3 position    = cameraTransform.mPosition;
        Fixed   forwardAxis = Fixed::FromFloat( Input::Get().Manager().GetAxis( "editor_forward" ) );
        Fixed   upAxis      = Fixed::FromFloat( Input::Get().Manager().GetAxis( "editor_up" ) );
        Fixed   leftAxis    = Fixed::FromFloat( Input::Get().Manager().GetAxis( "editor_left" ) );
        Fixed   boost       = Fixed::FromFloat( Input::Get().Manager().GetAxis( "editor_boost" ) );

        // Calculates speed
        Fixed realSpeed = editorCamera.mSpeed;
        if( boost > 0 )
        {
            realSpeed *= editorCamera.mSpeedMultiplier;
        }

        position += _delta * realSpeed * leftAxis * cameraTransform.Left();        // Camera goes left
        position += _delta * realSpeed * upAxis * cameraTransform.Up();            // Camera goes up
        position += _delta * realSpeed * forwardAxis * cameraTransform.Forward();    // Camera goes forward

        // Camera rotation
        const glm::vec2 mouseDelta = mouse.mPositionDelta;
        if( mouse.mDown[Mouse::button2] )
        {
            // Rotation depending on mouse movement
            const Fixed      invertAxis = -1;
            const Quaternion rotationY  = Quaternion::AngleAxis( Fixed::FromFloat( -editorCamera.mXYSensitivity.x * mouseDelta.x ), Vector3::sUp );
            const Quaternion rotationX  = Quaternion::AngleAxis( Fixed::FromFloat( -editorCamera.mXYSensitivity.y * mouseDelta.y ), invertAxis * cameraTransform.Left() );
            cameraTransform.mRotation = rotationX * rotationY * cameraTransform.mRotation;

            // Remove roll
            const Vector3 relativeLeft = cameraTransform.Left();
            const Vector3 leftNoRoll( relativeLeft.x, 0, relativeLeft.z );
            const Vector3 axis         = Vector3::Cross( relativeLeft, leftNoRoll );
            const Fixed   angle        = Vector3::Angle( leftNoRoll, relativeLeft );
            if( angle != 0 )
            {
                const Quaternion rot = Quaternion::AngleAxis( angle, axis );
                cameraTransform.mRotation = rot * cameraTransform.mRotation;
            }
        }
        cameraTransform.mPosition  = position;
    }

    //==================================================================================================================================================================================================
    // creates the editor camera entity & components
    // setups the EditorCamera singleton
    //==================================================================================================================================================================================================
    void EditorCamera::CreateEditorCamera( EcsWorld& _world )
    {
        Scene& scene = _world.GetSingleton<Scene>();

        // Editor Camera
        SceneNode& cameraNode = scene.CreateSceneNode( "editor_camera", &scene.GetRootNode() );
        EcsEntity cameraID = _world.GetEntity( cameraNode.mHandle );
        cameraNode.AddFlag( SceneNode::NoSave | SceneNode::NoDelete | SceneNode::NoRaycast );

        Transform& transform = _world.AddComponent<Transform>( cameraID );
        _world.AddComponent<Camera>( cameraID );

        transform.mPosition     = Vector3( 0, 0, -2 );
        scene.mMainCameraHandle = cameraNode.mHandle;

        // set editor camera singleton
        EditorCamera& editorCamera = _world.GetSingleton<EditorCamera>();
        editorCamera.mCameraHandle = cameraNode.mHandle;
    }
}