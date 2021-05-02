#include "game/fanProjectVoxels.hpp"
#include "core/math/fanFixedPoint.hpp"
#include "core/random/fanSimplexNoise.hpp"
#include "core/math/fanQuaternion.hpp"
#include "core/time/fanScopedTimer.hpp"
#include "network/singletons/fanTime.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/physics/fanUpdateFxRigidbodies.hpp"
#include "engine/systems/fanUpdateBounds.hpp"
#include "engine/systems/fanUpdateUIText.hpp"
#include "engine/systems/fanRaycastUI.hpp"
#include "engine/systems/fanUpdateUILayouts.hpp"
#include "engine/systems/fanUpdateUIAlign.hpp"
#include "engine/systems/fanUpdateTransforms.hpp"
#include "engine/systems/fanSynchronizeMotionStates.hpp"
#include "engine/singletons/fanPhysicsWorld.hpp"
#include "engine/systems/fanUpdateTimers.hpp"
#include "engine/systems/fanUpdateParticles.hpp"
#include "engine/systems/fanEmitParticles.hpp"
#include "engine/systems/fanGenerateParticles.hpp"
#include "engine/systems/fanRegisterPhysics.hpp"
#include "engine/systems/fanUpdateRenderWorld.hpp"
#include "engine/physics/fanDetectCollisions.hpp"
#include "engine/singletons/fanVoxelTerrain.hpp"

#include "game/components/fanTestComponent.hpp"
#include "game/singletons/fanTestSingleton.hpp"
#include "game/systems/fanTestSystem.hpp"

#ifdef FAN_EDITOR
#include "editor/fanModals.hpp"
#include "editor/singletons/fanGuiTestSingleton.hpp"
#include "editor/components/fanGuiTestComponent.hpp"
#include "editor/gui/scene/singletons/fanGuiVoxelTerrain.hpp"

#endif

namespace fan
{
    //==========================================================================================================================
    //==========================================================================================================================
    ProjectVoxels::ProjectVoxels()
    {
        mName = "project_voxels";
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void ProjectVoxels::Init()
    {
        mWorld.AddComponentType<TestComponent>();
        mWorld.AddSingletonType<TestSingleton>();
        mWorld.AddSingletonType<VoxelTerrain>();

        RenderResources& renderResources = mWorld.GetSingleton<RenderResources>();
        VoxelTerrain   & terrain         = mWorld.GetSingleton<VoxelTerrain>();
        VoxelTerrain::InitializeTerrainMesh( terrain, renderResources );
        VoxelTerrain::GenerateTerrain( terrain );

#ifdef FAN_EDITOR
        EditorGuiInfo& guiInfos = mWorld.GetSingleton<EditorGuiInfo>();
        guiInfos.mSingletonInfos[TestSingleton::Info::sType] = GuiVoxelTerrain::GetInfo();
        guiInfos.mSingletonInfos[TestSingleton::Info::sType] = GuiTestSingleton::GetInfo();
        guiInfos.mComponentInfos[TestComponent::Info::sType] = GuiTestComponent::GetInfo();
#endif
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void ProjectVoxels::Start()
    {
        mWorld.Run<SRegisterAllRigidbodies>();
        MeshManager& meshManager = *mWorld.GetSingleton<RenderResources>().mMeshManager;
        RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
        meshManager.Add( renderWorld.mParticlesMesh, "particles_mesh_" + mName );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void ProjectVoxels::Stop()
    {
    }

    //============================================================================================================================
    //============================================================================================================================
    void ProjectVoxels::Step( const float _delta )
    {
        SCOPED_PROFILE( step );

        const Fixed fxDelta = Fixed::FromFloat( _delta );

        // physics & transforms
        PhysicsWorld& physicsWorld = mWorld.GetSingleton<PhysicsWorld>();
        mWorld.Run<SSynchronizeMotionStateFromTransform>();
        physicsWorld.mDynamicsWorld->stepSimulation( _delta, 10, Time::sPhysicsDelta );

        {
            mWorld.Run<SIntegrateFxRigidbodies>( fxDelta );
            mWorld.Run<SDetectCollisions>( fxDelta );
        }

        mWorld.Run<SSynchronizeTransformFromMotionState>();
        mWorld.Run<SMoveFollowTransforms>();

        // ui
        mWorld.Run<SUpdateUIText>();
        mWorld.Run<SAlignUI>();
        mWorld.Run<SUpdateUILayouts>();
        mWorld.Run<SHoverButtons>();
        mWorld.Run<SHighlightButtons>();

        // gameplay
        mWorld.Run<SUpdateExpirationTimes>( _delta );

        mWorld.Run<SUpdateParticles>( _delta );
        mWorld.Run<SEmitParticles>( _delta );
        mWorld.Run<SGenerateParticles>( _delta );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void ProjectVoxels::UpdateRenderWorld()
    {
        SCOPED_PROFILE( update_render_world );

        RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
        renderWorld.drawData.clear();

        mWorld.Run<SUpdateRenderWorldModelsFixed>( renderWorld );
        mWorld.Run<SUpdateRenderWorldModels>( renderWorld );
        mWorld.ForceRun<SUpdateRenderWorldUI>();
        mWorld.ForceRun<SUpdateRenderWorldPointLights>();
        mWorld.ForceRun<SUpdateRenderWorldDirectionalLights>();
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void ProjectVoxels::OnGui()
    {
        if( ImGui::Begin( "testoss" ) )
        {
            ImGui::End();
        }
    }
}
