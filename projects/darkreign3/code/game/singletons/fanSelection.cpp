#include "game/singletons/fanSelection.hpp"
#include "core/input/fanKeyboard.hpp"
#include "engine/singletons/fanMouse.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/components/fanCamera.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/systems/fanRaycast.hpp"
#include "game/components/fanUnit.hpp"
#include "game/systems/fanUpdateSelection.hpp"

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
        const Ray              ray = camera.ScreenPosToRay( cameraTransform, mouse.LocalScreenSpacePosition() );
        std::vector<EcsEntity> results;
        Raycast<Unit>( _world, ray, results );

        // set hover target type
        if( !results.empty() )
        {
            EcsEntity entity = results[0];
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
                EcsEntity entity = results[0];
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
        return selectionStatus;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Selection::Save( const EcsSingleton& _singleton, Json& _json )
    {
        const Selection& selection = static_cast<const Selection&>( _singleton );
        Serializable::SaveResourcePtr( _json, "selection_frame", selection.mSelectionFramePrefab.mData );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Selection::Load( EcsSingleton& _singleton, const Json& _json )
    {
        Selection& selection = static_cast<Selection&>( _singleton );
        Serializable::LoadResourcePtr( _json, "selection_frame", selection.mSelectionFramePrefab.mData );
    }
}