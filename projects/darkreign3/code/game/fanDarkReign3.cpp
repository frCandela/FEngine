#include "game/fanDarkReign3.hpp"
#include "platform/input/fanKeyboard.hpp"
#include "platform/input/fanInput.hpp"
#include "platform/input/fanInputManager.hpp"
#include "core/random/fanSimplexNoise.hpp"
#include "core/time/fanScopedTimer.hpp"
#include "network/singletons/fanTime.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/physics/fanUpdateRigidbodies.hpp"
#include "engine/systems/fanUpdateBounds.hpp"
#include "engine/systems/fanUpdateTransforms.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/singletons/fanApplication.hpp"
#include "engine/systems/fanUpdateTimers.hpp"
#include "engine/systems/fanUpdateParticles.hpp"
#include "engine/systems/fanEmitParticles.hpp"
#include "engine/systems/fanGenerateParticles.hpp"
#include "engine/systems/fanUpdateAnimators.hpp"
#include "engine/physics/fanDetectCollisions.hpp"
#include "engine/terrain/fanVoxelTerrain.hpp"
#include "engine/components/fanCamera.hpp"

#include "game/components/fanUnit.hpp"
#include "game/components/fanAnimScale.hpp"
#include "game/components/fanTerrainAgent.hpp"
#include "game/units/fanJudas.hpp"
#include "game/singletons/fanSelection.hpp"
#include "game/singletons/fanRTSCamera.hpp"
#include "game/singletons/fanPauseMenu.hpp"
#include "game/systems/fanUpdateSelection.hpp"
#include "game/systems/fanUpdateAnimScale.hpp"
#include "game/systems/fanUpdateAgents.hpp"
#include "game/systems/fanUpdateUnits.hpp"
#include "game/systems/fanUpdateJudas.hpp"
#include "game/fanDR3Tags.hpp"

#ifdef FAN_EDITOR
#include "editor/singletons/fanEditorSettings.hpp"
#include "editor/fanGuiSelection.hpp"
#include "editor/fanGuiRTSCamera.hpp"
#include "editor/fanGuiPauseMenu.hpp"
#include "editor/fanGuiUnit.hpp"
#include "editor/fanGuiAnimScale.hpp"
#include "editor/fanGuiTerrainAgent.hpp"
#include "editor/fanGuiJudas.hpp"
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
        mWorld.AddComponentType<Judas>();
        mWorld.AddSingletonType<Selection>();
        mWorld.AddSingletonType<RTSCamera>();
        mWorld.AddSingletonType<PauseMenu>();
        mWorld.AddTagType<TagSelected>();
        mWorld.AddTagType<TagEnemy>();
        mWorld.AddTagType<TagUnitStateNeedsUpdate>();

#ifdef FAN_EDITOR
        EditorSettings& settings = mWorld.GetSingleton<EditorSettings>();
        settings.mSingletonInfos[Selection::Info::sType]    = GuiSelection::GetInfo();
        settings.mSingletonInfos[RTSCamera::Info::sType]    = GuiRTSCamera::GetInfo();
        settings.mSingletonInfos[PauseMenu::Info::sType]    = GuiPauseMenu::GetInfo();
        settings.mComponentInfos[Unit::Info::sType]         = GuiUnit::GetInfo();
        settings.mComponentInfos[AnimScale::Info::sType]    = GuiAnimScale::GetInfo();
        settings.mComponentInfos[TerrainAgent::Info::sType] = GuiTerrainAgent::GetInfo();
        settings.mComponentInfos[Judas::Info::sType]        = GuiJudas::GetInfo();
        mWorld.GetSingleton<Application>().mOnEditorUseGameCamera.Connect( &DarkReign3::OnEditorUseGameCamera, this );
#endif

        mCursors.Load( *mWorld.GetSingleton<Application>().mResources );
        Input::Get().Manager().CreateKeyboardEvent( "game_pause", Keyboard::ESCAPE )->Connect( &DarkReign3::OnTogglePause, this );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::Start()
    {
        RTSCamera::Instantiate( mWorld );
        PauseMenu::Instantiate( mWorld );
        PauseMenu::Hide( mWorld );
        mPaused = false;

        PauseMenu& pauseMenu = mWorld.GetSingleton<PauseMenu>();
        pauseMenu.mOnResume.Connect( &DarkReign3::OnResume, this );
        pauseMenu.mOnQuit.Connect( &DarkReign3::OnQuit, this );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::Stop()
    {
        PauseMenu::Destroy( mWorld );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::OnEditorUseGameCamera()
    {
        RTSCamera& rtsCamera = mWorld.GetSingleton<RTSCamera>();
        fanAssert( rtsCamera.mCameraHandle != 0 );
        mWorld.GetSingleton<Scene>().SetMainCamera( rtsCamera.mCameraHandle );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::OnTogglePause()
    {
        Debug::Log( "DarkReign3::OnTogglePause" );
        if( mPaused )
        {
            OnResume();
        }
        else
        {
            OnPause();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::OnPause()
    {
        if( !mPaused )
        {
            mPaused = true;
            mWorld.GetSingleton<Time>().mLogicTimeScale = 0;
            PauseMenu::Show( mWorld );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::OnResume()
    {
        if( mPaused )
        {
            mPaused = false;
            mWorld.GetSingleton<Time>().mLogicTimeScale = 1;
            PauseMenu::Hide( mWorld );
        }
    }
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::OnQuit()
    {
        mWorld.GetSingleton<Application>().mOnQuit.Emmit();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::PreStep( const Fixed _delta )
    {
        (void)_delta;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::Step( const Fixed _delta )
    {
        SCOPED_PROFILE( step );

        static const int chunksPerFrame = System::GetBuildType() == System::BuildType::Release ? 16 : 1;
        VoxelTerrain::StepLoadTerrain( mWorld, chunksPerFrame );

        RTSCamera::Update( mWorld, _delta );

        // update selection
        const SelectionStatus selectionStatus = Selection::SelectUnits( mWorld, _delta );
        mWorld.Run<SUpdateUnitsState>();
        mWorld.Run<SMoveAgents>( _delta );

        // set cursor
        Application& app = mWorld.GetSingleton<Application>();
        const DR3Cursor currentCursor = DR3Cursors::GetCurrentCursor( _delta, mWorld.GetSingleton<Time>(), selectionStatus );
        app.mCurrentCursor = currentCursor == DR3Cursor::Count ? nullptr : mCursors.mCursors[currentCursor];

        mWorld.Run<SUpdateJudasAnimation>( _delta );

        // ui
        mWorld.Run<SUpdateAnimScale>( _delta );
        mWorld.ForceRun<SPlaceSelectionFrames>( _delta );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::OnGui()
    {
        if( ImGui::Begin( "testosss" ) )
        {
            if( Keyboard::Get().IsKeyPressed( Keyboard::T ) )
            {
                EcsEntity cameraID = mWorld.GetEntity( mWorld.GetSingleton<Scene>().mMainCameraHandle );
                Mouse          & mouse           = mWorld.GetSingleton<Mouse>();
                const Transform& cameraTransform = mWorld.GetComponent<Transform>( cameraID );
                const Camera   & camera          = mWorld.GetComponent<Camera>( cameraID );
                const Ray                     mousePosRay = camera.ScreenPosToRay( cameraTransform, mouse.LocalScreenSpacePosition() );
                std::vector<SRaycast::Result> results;
                Raycast<MeshRenderer>( mWorld, mousePosRay, results );
                if( !results.empty() )
                {
                    SRaycast::Result result = results[0];
                    if( mWorld.HasTag<TagEnemy>( result.mEntity ) )
                    {
                        mWorld.RemoveTag<TagEnemy>( result.mEntity );
                        Debug::Log( "untag" );
                    }
                    else
                    {
                        mWorld.AddTag<TagEnemy>( result.mEntity );
                        Debug::Log( "tag" );
                    }
                }
            }
        }
        ImGui::End();
    }
}