#include "game/fanDarkReign3.hpp"
#include "core/random/fanSimplexNoise.hpp"
#include "core/time/fanScopedTimer.hpp"
#include "core/input/fanKeyboard.hpp"
#include "network/singletons/fanTime.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/physics/fanUpdateRigidbodies.hpp"
#include "engine/systems/fanUpdateBounds.hpp"
#include "engine/systems/fanUpdateUIText.hpp"
#include "engine/systems/fanRaycastUI.hpp"
#include "engine/systems/fanUpdateUILayouts.hpp"
#include "engine/systems/fanUpdateUIAlign.hpp"
#include "engine/systems/fanUpdateTransforms.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/systems/fanUpdateTimers.hpp"
#include "engine/systems/fanUpdateParticles.hpp"
#include "engine/systems/fanEmitParticles.hpp"
#include "engine/systems/fanGenerateParticles.hpp"
#include "engine/systems/fanUpdateRenderWorld.hpp"
#include "engine/physics/fanDetectCollisions.hpp"
#include "engine/terrain/fanVoxelTerrain.hpp"
#include "engine/components/fanCamera.hpp"
#include "engine/systems/fanRaycast.hpp"

#include "game/components/fanUnit.hpp"
#include "game/components/fanAnimScale.hpp"
#include "game/components/fanTerrainAgent.hpp"
#include "game/singletons/fanSelection.hpp"
#include "game/fanDR3Tags.hpp"
#include "game/systems/fanUpdateSelection.hpp"
#include "game/systems/fanUpdateAnimScale.hpp"
#include "game/systems/fanUpdateAgents.hpp"

#ifdef FAN_EDITOR
#include "editor/singletons/fanEditorSettings.hpp"
#include "editor/fanGuiSelection.hpp"
#include "editor/fanGuiUnit.hpp"
#include "editor/fanGuiAnimScale.hpp"
#include "editor/fanGuiTerrainAgent.hpp"
#endif

#include "render/fanWindow.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::Init()
    {
        mWorld.AddComponentType<Unit>();
        mWorld.AddComponentType<TerrainAgent>();
        mWorld.AddComponentType<AnimScale>();
        mWorld.AddSingletonType<Selection>();
        mWorld.AddTagType<TagSelected>();
        mWorld.AddTagType<TagEnemy>();

#ifdef FAN_EDITOR
        EditorSettings& settings = mWorld.GetSingleton<EditorSettings>();
        settings.mSingletonInfos[Selection::Info::sType] = GuiSelection::GetInfo();
        settings.mComponentInfos[Unit::Info::sType]      = GuiUnit::GetInfo();
        settings.mComponentInfos[AnimScale::Info::sType] = GuiAnimScale::GetInfo();
        settings.mComponentInfos[TerrainAgent::Info::sType] = GuiTerrainAgent::GetInfo();
#endif

        mCursors.Load( *mWorld.GetSingleton<Application>().mResources );
        mWorld.GetSingleton<Application>().mOnEditorUseGameCamera.Connect( &DarkReign3::OnEditorUseGameCamera, this );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::Start()
    {
        Scene& scene = mWorld.GetSingleton<Scene>();
        SceneNode cameraNode = scene.CreateSceneNode( "game_camera", &scene.GetRootNode() );
        mGameCameraHandle = cameraNode.mHandle;
        EcsEntity cameraEntity = mWorld.GetEntity( mGameCameraHandle );
        mWorld.AddComponent<Camera>( cameraEntity );
        Transform& cameraTransform = mWorld.AddComponent<Transform>( cameraEntity );

        if( scene.mMainCameraHandle != 0 )
        {
            Transform& prevCameraTransform = mWorld.GetComponent<Transform>( mWorld.GetEntity( scene.mMainCameraHandle ) );
            cameraTransform = prevCameraTransform;
        }
        scene.SetMainCamera( mGameCameraHandle );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::Stop()
    {
    }

    //============================================================================================================================
    //============================================================================================================================
    void DarkReign3::OnEditorUseGameCamera()
    {
        fanAssert( mGameCameraHandle != 0 );
        mWorld.GetSingleton<Scene>().SetMainCamera( mGameCameraHandle );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::Step( const Fixed _delta )
    {
        SCOPED_PROFILE( step );

        // load terrain
        static const int chunksPerFrame = System::GetBuildType() == System::BuildType::Release ? 16 : 1;
        VoxelTerrain::StepLoadTerrain( mWorld, chunksPerFrame );

        // update selection
        const SelectionStatus selectionStatus = Selection::SelectUnits( mWorld, _delta );
        mWorld.Run<SMoveAgents>(_delta);

        // set cursor
        Application& app = mWorld.GetSingleton<Application>();
        const DR3Cursor currentCursor = DR3Cursors::GetCurrentCursor( _delta, mWorld.GetSingleton<Time>(), selectionStatus );
        app.mCurrentCursor = currentCursor == DR3Cursor::Count ? nullptr : mCursors.mCursors[currentCursor];

        // physics & transforms
        mWorld.Run<SIntegrateRigidbodies>( _delta );
        mWorld.Run<SDetectCollisions>( _delta );
        mWorld.Run<SMoveFollowTransforms>();

        // ui
        mWorld.Run<SUpdateAnimScale>( _delta );
        mWorld.ForceRun<SPlaceSelectionFrames>( _delta );
        mWorld.Run<SUpdateUIText>();
        mWorld.Run<SAlignUI>();
        mWorld.Run<SUpdateUILayouts>();
        mWorld.Run<SHoverButtons>();
        mWorld.Run<SHighlightButtons>();

        // gameplay
        mWorld.Run<SUpdateExpirationTimes>( _delta.ToFloat() );
        mWorld.Run<SUpdateParticles>( _delta.ToFloat() );
        mWorld.Run<SEmitParticles>( _delta.ToFloat() );
        mWorld.Run<SGenerateParticles>( _delta.ToFloat() );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::Render()
    {
        SCOPED_PROFILE( update_render_world );

        RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
        renderWorld.drawData.clear();

        mWorld.Run<SUpdateRenderWorldModels>( renderWorld );
        mWorld.ForceRun<SUpdateRenderWorldUI>();
        mWorld.ForceRun<SUpdateRenderWorldPointLights>();
        mWorld.ForceRun<SUpdateRenderWorldDirectionalLights>();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::OnGui()
    {
        if( ImGui::Begin( "testoss" ) )
        {
            if( ImGui::Button( "tag enemy" ) )
            {
                EcsView view   = mWorld.Match<SClearSelection>();
                auto    unitIt = view.begin<Unit>();
                for( ; unitIt != view.end<Unit>(); ++unitIt )
                {
                    mWorld.AddTag<TagEnemy>( unitIt.GetEntity() );
                }
            }
            if( ImGui::Button( "tag terrain" ) )
            {
                EcsView view   = mWorld.Match<SClearSelection>();
                auto    unitIt = view.begin<Unit>();
                for( ; unitIt != view.end<Unit>(); ++unitIt )
                {
                    mWorld.AddTag<TagTerrain>( unitIt.GetEntity() );
                }
            }
        }
        ImGui::End();
    }
}
