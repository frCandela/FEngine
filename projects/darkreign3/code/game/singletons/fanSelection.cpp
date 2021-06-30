#include "game/singletons/fanSelection.hpp"
#include "platform/input/fanKeyboard.hpp"
#include "core/math/fanMatrix3.hpp"
#include "engine/singletons/fanMouse.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/components/fanCamera.hpp"
#include "engine/components/fanScale.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/systems/fanRaycast.hpp"
#include "engine/fanEngineTags.hpp"
#include "game/components/fanUnit.hpp"
#include "game/systems/fanUpdateSelection.hpp"
#include "game/systems/fanUpdateAgents.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Selection::SetInfo( EcsSingletonInfo& _info )
    {
        _info.save = &Selection::Save;
        _info.load = &Selection::Load;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Selection::Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        Selection& selection = static_cast<Selection&>( _singleton );
        selection.mSelectionFramePrefab = nullptr;
        selection.mMoveToFxPrefab       = nullptr;
        selection.mSelectionFrames.clear();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    SelectionStatus Selection::SelectUnits( EcsWorld& _world, const Fixed _delta )
    {
        if( _delta == 0 ){ return SelectionStatus(); }

        Mouse& mouse = _world.GetSingleton<Mouse>();
        if( !mouse.mWindowHovered ){ return SelectionStatus(); }

        SelectionStatus selectionStatus;
        selectionStatus.mNumSelected = _world.Match<SClearSelection>().Size();

        if( Keyboard::IsKeyPressed( Keyboard::ESCAPE ) )
        {
            _world.Run<SClearSelection>();
        }

        // raycast on all units
        EcsEntity cameraID = _world.GetEntity( _world.GetSingleton<Scene>().mMainCameraHandle );
        const Transform& cameraTransform = _world.GetComponent<Transform>( cameraID );
        const Camera   & camera          = _world.GetComponent<Camera>( cameraID );
        const Ray                  mousePosRay = camera.ScreenPosToRay( cameraTransform, mouse.LocalScreenSpacePosition() );
        std::vector<SRaycast::Result> results;
        Raycast<Unit>( _world, mousePosRay, results );

        // set hover target type
        if( !results.empty() )
        {
            EcsEntity entity = results[0].mEntity;
            if( _world.HasTag<TagEnemy>( entity ) )
            {
                selectionStatus.mHoveringOverEnemy = true;
            }
            else
            {
                selectionStatus.mHoveringOverAlly = true;
            }
        }

        // select / deselect on left click
        if( mouse.mPressed[Mouse::buttonLeft] )
        {
            if( results.empty() )
            {
                if( !Keyboard::IsKeyDown( Keyboard::LEFT_CONTROL ) )
                {
                    _world.Run<SClearSelection>();
                }
            }
            else
            {
                EcsEntity entity = results[0].mEntity;
                if( !_world.HasTag<TagEnemy>( entity ) )
                {
                    if( _world.HasTag<TagSelected>( entity ) )
                    {
                        _world.RemoveTag<TagSelected>( entity );
                    }
                    else
                    {
                        if( !Keyboard::IsKeyDown( Keyboard::LEFT_CONTROL ) )
                        {
                            _world.Run<SClearSelection>();
                        }
                        _world.AddTag<TagSelected>( entity );
                    }
                }
            }
        }
        else if( mouse.mPressed[Mouse::buttonRight] )
        {
            // move unit command
            if( selectionStatus.mNumSelected > 0 )
            {
                results.clear();
                Raycast<TagTerrain>( _world, mousePosRay, results );
                if( !results.empty() )
                {
                    const Scene    & scene      = _world.GetSingleton<Scene>();
                    const Selection& _selection = _world.GetSingleton<Selection>();
                    SceneNode      * node       = _selection.mMoveToFxPrefab->Instantiate( scene.GetRootNode() );
                    Transform      & transform  = _world.GetComponent<Transform>( _world.GetEntity( node->mHandle ) );
                    transform.mPosition = results[0].mData.mPosition - 2 * mousePosRay.direction;

                    const Vector3 up = results[0].mData.mNormal.Normalized();
                    const Vector3 left = Vector3::Cross( up, transform.Forward() );
                    const Vector3 forward = Vector3::Cross( left, up );
                    transform.mRotation = Matrix3(left, up, forward).ToQuaternion();

                    _world.Run<SSetSelectedAgentsDestination>(results[0].mData.mPosition);
                }
            }
        }

        return selectionStatus;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Selection::Save( const EcsSingleton& _singleton, Json& _json )
    {
        const Selection& selection = static_cast<const Selection&>( _singleton );
        Serializable::SaveResourcePtr( _json, "selection_frame", selection.mSelectionFramePrefab );
        Serializable::SaveResourcePtr( _json, "move_to", selection.mMoveToFxPrefab );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Selection::Load( EcsSingleton& _singleton, const Json& _json )
    {
        Selection& selection = static_cast<Selection&>( _singleton );
        Serializable::LoadResourcePtr( _json, "selection_frame", selection.mSelectionFramePrefab );
        Serializable::LoadResourcePtr( _json, "move_to", selection.mMoveToFxPrefab );
    }
}