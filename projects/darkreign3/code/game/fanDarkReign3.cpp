#include "game/fanDarkReign3.hpp"
#include "core/random/fanSimplexNoise.hpp"
#include "core/time/fanScopedTimer.hpp"
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
#include "game/singletons/fanSelection.hpp"
#include "game/fanDR3Tags.hpp"
#include "game/systems/fanUpdateSelection.hpp"

#ifdef FAN_EDITOR
#include "editor/singletons/fanEditorSettings.hpp"
#include "editor/fanGuiSelection.hpp"
#include "editor/fanGuiUnit.hpp"

#endif

#include "render/fanWindow.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::Init()
    {
        mWorld.AddComponentType<Unit>();
        mWorld.AddSingletonType<Selection>();
        mWorld.AddTagType<TagSelected>();

#ifdef FAN_EDITOR
        EditorSettings& settings = mWorld.GetSingleton<EditorSettings>();
        settings.mSingletonInfos[Selection::Info::sType] = GuiSelection::GetInfo();
        settings.mComponentInfos[Unit::Info::sType]      = GuiUnit::GetInfo();
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

    int max;
    int completionVoxelsGeneration;
    int completionMeshGeneration;
    int chunksPerFrame = System::GetBuildType() == System::BuildType::Release ? 16 : 1;

    //============================================================================================================================
    //============================================================================================================================
    void DarkReign3::OnEditorUseGameCamera()
    {
        fanAssert( mGameCameraHandle != 0 );
        mWorld.GetSingleton<Scene>().SetMainCamera( mGameCameraHandle );
    }

    //============================================================================================================================
    //============================================================================================================================
    void DarkReign3::StepLoadTerrain()
    {
        VoxelTerrain& terrain   = mWorld.GetSingleton<VoxelTerrain>();
        Resources   & resources = *mWorld.GetSingleton<Application>().mResources;
        if( !terrain.mIsInitialized ){ return; }

        for( int iteration = 0; iteration < chunksPerFrame; ++iteration )
        {
            max = terrain.mSize.x * terrain.mSize.y * terrain.mSize.z;

            // load/generate blocks
            bool     generatedBlocks = false;
            for( int i               = 0; i < terrain.mSize.x * terrain.mSize.y * terrain.mSize.z; i++ )
            {
                VoxelChunk& chunk = terrain.mChunks[i];
                if( !chunk.mIsGenerated )
                {
                    VoxelGenerator::GenerateBlocks( terrain, chunk );
                    completionVoxelsGeneration = i;
                    completionMeshGeneration   = 0;
                    generatedBlocks            = true;
                    break;
                }
            }
            if( generatedBlocks ){ continue; }

            // generate mesh
            for( int i = 0; i < terrain.mSize.x * terrain.mSize.y * terrain.mSize.z; i++ )
            {
                VoxelChunk& chunk = terrain.mChunks[i];
                if( chunk.mIsMeshOutdated )
                {
                    EcsEntity entity = mWorld.GetEntity( chunk.mHandle );
                    MeshRenderer& meshRenderer = mWorld.GetComponent<MeshRenderer>( entity );
                    SceneNode   & sceneNode    = mWorld.GetComponent<SceneNode>( entity );
                    sceneNode.AddFlag( SceneNode::BoundsOutdated );
                    VoxelGenerator::GenerateMesh( terrain, chunk, ( meshRenderer.mMesh )->mSubMeshes[0] );
                    meshRenderer.mMesh->GenerateBoundingVolumes();
                    resources.SetDirty( meshRenderer.mMesh->mGUID );
                    completionMeshGeneration = i;
                    break;
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SelectUnits( EcsWorld& _world, const Fixed _delta )
    {
        if( _delta > 0 )
        {
            Mouse& mouse = _world.GetSingleton<Mouse>();
            if( mouse.mPressed[Mouse::buttonLeft] )
            {
                _world.Run<SClearSelection>();

                EcsEntity cameraID = _world.GetEntity( _world.GetSingleton<Scene>().mMainCameraHandle );
                const Transform& cameraTransform = _world.GetComponent<Transform>( cameraID );
                const Camera   & camera          = _world.GetComponent<Camera>( cameraID );
                const Ray              ray = camera.ScreenPosToRay( cameraTransform, mouse.LocalScreenSpacePosition() );
                std::vector<EcsEntity> results;
                Raycast<Unit>( _world, ray, results );
                if( !results.empty() )
                {
                    EcsEntity entity = results[0];
                    _world.AddTag<TagSelected>( entity );
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::Step( const Fixed _delta )
    {
        SCOPED_PROFILE( step );

        StepLoadTerrain();
        SelectUnits( mWorld, _delta );
        mWorld.ForceRun<SPlaceSelectionFrames>( _delta );

        // physics & transforms
        mWorld.Run<SIntegrateRigidbodies>( _delta );
        mWorld.Run<SDetectCollisions>( _delta );
        mWorld.Run<SMoveFollowTransforms>();

        // ui
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
            ImGui::DragInt( "chunks per frame", &chunksPerFrame, 1, 1, 100 );
            int completionVoxelsGenerationCpy = completionVoxelsGeneration;
            ImGui::SliderInt( "voxels generation", &completionVoxelsGenerationCpy, 0, max );
            int completionMeshGenerationCpy = completionMeshGeneration;
            ImGui::SliderInt( "mesh generation", &completionMeshGenerationCpy, 0, max );
        }
        ImGui::End();
    }
}
