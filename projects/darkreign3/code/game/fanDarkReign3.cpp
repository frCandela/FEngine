#include <editor/gui/fanGuiRenderResources.hpp>
#include "game/fanDarkReign3.hpp"
#include "core/random/fanSimplexNoise.hpp"
#include "core/math/fanQuaternion.hpp"
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

#include "game/components/fanTestComponent.hpp"
#include "game/singletons/fanTestSingleton.hpp"
#include "game/systems/fanTestSystem.hpp"

#ifdef FAN_EDITOR
#include "editor/singletons/fanEditorSettings.hpp"
#include "editor/fanGuiTestSingleton.hpp"
#include "editor/fanGuiTestComponent.hpp"
#include "editor/fanModals.hpp"
#endif

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::Init()
    {
        mWorld.AddComponentType<TestComponent>();
        mWorld.AddSingletonType<TestSingleton>();

#ifdef FAN_EDITOR
        EditorSettings& settings = mWorld.GetSingleton<EditorSettings>();
        settings.mSingletonInfos[TestSingleton::Info::sType] = GuiTestSingleton::GetInfo();
        settings.mComponentInfos[TestComponent::Info::sType] = GuiTestComponent::GetInfo();
#endif
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::Start()
    {
        /*ResourceManager& resources   = *mWorld.GetSingleton<EngineResources>().mResources;
        RenderWorld    & renderWorld = mWorld.GetSingleton<RenderWorld>();*/

        Scene& scene = mWorld.GetSingleton<Scene>();
        SceneNode cameraNode = scene.CreateSceneNode( "game_camera", &scene.GetRootNode() );

        EcsEntity cameraEntity = mWorld.GetEntity( cameraNode.mHandle );
        mWorld.AddComponent<Camera>( cameraEntity );
        Transform& cameraTransform = mWorld.AddComponent<Transform>( cameraEntity );

        if( scene.mMainCameraHandle != 0 )
        {
            Transform& prevCameraTransform = mWorld.GetComponent<Transform>( mWorld.GetEntity( scene.mMainCameraHandle ) );
            cameraTransform = prevCameraTransform;
        }

        scene.SetMainCamera( cameraNode.mHandle );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DarkReign3::Stop()
    {
    }

    int max;
    int completionVoxelsGeneration;
    int completionMeshGeneration;
    int chunksPerFrame = System::GetBuildType() == System::BuildType::Release ? 32 : 4;

    //============================================================================================================================
    //============================================================================================================================
    void DarkReign3::StepLoadTerrain()
    {
        VoxelTerrain   & terrain   = mWorld.GetSingleton<VoxelTerrain>();
        ResourceManager& resources = *mWorld.GetSingleton<EngineResources>().mResources;
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
                    ( meshRenderer.mMesh )->GenerateConvexHull();
                    resources.SetDirty( meshRenderer.mMesh->mGUID );
                    completionMeshGeneration = i;
                    break;
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

        //const Time& time = mWorld.GetSingleton<Time>();

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
