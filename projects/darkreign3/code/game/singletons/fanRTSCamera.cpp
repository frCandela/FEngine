#include "game/singletons/fanRTSCamera.hpp"
#include "core/input/fanKeyboard.hpp"
#include "engine/fanEngineSerializable.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/components/fanCamera.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/singletons/fanMouse.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RTSCamera::SetInfo( EcsSingletonInfo& _info )
    {
        _info.save = &RTSCamera::Save;
        _info.load = &RTSCamera::Load;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RTSCamera::Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        RTSCamera& rtsCamera = static_cast<RTSCamera&>( _singleton );
        rtsCamera.mMinHeight        = 50;
        rtsCamera.mMaxHeight        = 100;
        rtsCamera.mCameraHandle     = 0;
        rtsCamera.mZoomSpeed        = 500;
        rtsCamera.mTranslationSpeed = 50;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RTSCamera::CreateCamera( EcsWorld& _world )
    {
        RTSCamera& rtsCamera = _world.GetSingleton<RTSCamera>();
        Scene    & scene     = _world.GetSingleton<Scene>();
        SceneNode cameraNode = scene.CreateSceneNode( "rts_camera", &scene.GetRootNode() );
        rtsCamera.mCameraHandle = cameraNode.mHandle;
        EcsEntity cameraEntity = _world.GetEntity( rtsCamera.mCameraHandle );
        _world.AddComponent<Camera>( cameraEntity );
        Transform& cameraTransform = _world.AddComponent<Transform>( cameraEntity );
        cameraTransform.mPosition = Vector3( 0, 75, 0 );
        cameraTransform.mRotation = Quaternion::Euler( 45, 0, 0 );
        scene.SetMainCamera( rtsCamera.mCameraHandle );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RTSCamera::Update( EcsWorld& _world, const Fixed _delta )
    {
        if( _delta > 0 )
        {
            RTSCamera& rtsCamera = _world.GetSingleton<RTSCamera>();
            EcsEntity entity = _world.GetEntity( rtsCamera.mCameraHandle );
            Transform& transform = _world.GetComponent<Transform>( entity );
            Mouse    & mouse     = _world.GetSingleton<Mouse>();

            // height controll
            if( mouse.mScrollDelta != glm::vec2 { 0, 0 } )
            {
                const Vector3 forward = transform.Forward();
                const Vector3 delta   = _delta * Fixed::FromFloat( mouse.mScrollDelta.y ) * rtsCamera.mZoomSpeed * forward;
                transform.mPosition += delta;

                // correct height
                if( transform.mPosition.y > rtsCamera.mMaxHeight )
                {
                    Fixed overHeight = transform.mPosition.y - rtsCamera.mMaxHeight;
                    transform.mPosition -= ( overHeight / forward.y ) * forward;;
                }
                else if( transform.mPosition.y < rtsCamera.mMinHeight )
                {
                    Fixed underHeight = rtsCamera.mMinHeight - transform.mPosition.y;
                    transform.mPosition += ( underHeight / forward.y ) * forward;
                }
            }

            // pan with arrows
            Vector3 horizontalForward = transform.Forward();
            horizontalForward.y = 0;
            horizontalForward.Normalize();
            const Fixed xAxis = Keyboard::IsKeyDown( Keyboard::LEFT ) ? 1 : Keyboard::IsKeyDown( Keyboard::RIGHT ) ? -1 : 0;
            const Fixed yAxis = Keyboard::IsKeyDown( Keyboard::UP ) ? 1 : Keyboard::IsKeyDown( Keyboard::DOWN ) ? -1 : 0;
            transform.mPosition += _delta * rtsCamera.mTranslationSpeed * xAxis * transform.Left();
            transform.mPosition += _delta * rtsCamera.mTranslationSpeed * yAxis * horizontalForward;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RTSCamera::Save( const EcsSingleton& _singleton, Json& _json )
    {
        const RTSCamera& rtsCamera = static_cast<const RTSCamera&>( _singleton );
        Serializable::SaveFixed( _json, "min height", rtsCamera.mMinHeight );
        Serializable::SaveFixed( _json, "max height", rtsCamera.mMaxHeight );
        Serializable::SaveFixed( _json, "zoom speed", rtsCamera.mZoomSpeed );
        Serializable::SaveFixed( _json, "translation_speed", rtsCamera.mTranslationSpeed );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RTSCamera::Load( EcsSingleton& _singleton, const Json& _json )
    {
        RTSCamera& rtsCamera = static_cast<RTSCamera&>( _singleton );
        Serializable::LoadFixed( _json, "min height", rtsCamera.mMinHeight );
        Serializable::LoadFixed( _json, "max height", rtsCamera.mMaxHeight );
        Serializable::LoadFixed( _json, "zoom speed", rtsCamera.mZoomSpeed );
        Serializable::LoadFixed( _json, "translation_speed", rtsCamera.mTranslationSpeed );
    }
}