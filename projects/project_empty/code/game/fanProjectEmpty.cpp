#include "game/fanProjectEmpty.hpp"

#include "core/math/fanFixedPoint.hpp"
#include "core/time/fanScopedTimer.hpp"
#include "network/singletons/fanTime.hpp"
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

#include "game/components/fanTestComponent.hpp"
#include "game/singletons/fanTestSingleton.hpp"

#include "editor/fanRegisterEditorGui.hpp"

namespace fan
{
    //==========================================================================================================================
    //==========================================================================================================================
    ProjectEmpty::ProjectEmpty()
    {
        mName = "project_empty";
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void ProjectEmpty::Init()
    {
        EcsIncludeEngine( mWorld );
        EcsIncludePhysics(mWorld);
        EcsIncludeRender3D(mWorld);
        EcsIncludeRenderUI(mWorld);

        mWorld.AddComponentType<TestComponent>();
        mWorld.AddSingletonType<TestSingleton>();

#ifdef FAN_EDITOR
        RegisterEditorGuiInfos( mWorld.GetSingleton<EditorGuiInfo>() );
#endif
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void ProjectEmpty::Start()
    {
        mWorld.Run<SRegisterAllRigidbodies>();
        MeshManager& meshManager = *mWorld.GetSingleton<RenderResources>().mMeshManager;
        RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
        meshManager.Add( renderWorld.mParticlesMesh, "particles_mesh_" + mName );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void  ProjectEmpty::Stop()
    {

    }

    //==========================================================================================================================
    //==========================================================================================================================
    void  ProjectEmpty::Step( const float _delta )
    {
        SCOPED_PROFILE( step );

        // physics & transforms
        PhysicsWorld& physicsWorld = mWorld.GetSingleton<PhysicsWorld>();
        mWorld.Run<SSynchronizeMotionStateFromTransform>();
        physicsWorld.mDynamicsWorld->stepSimulation( _delta, 10, Time::sPhysicsDelta );
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
    void ProjectEmpty::UpdateRenderWorld()
    {
        SCOPED_PROFILE( update_render_world );
        mWorld.ForceRun<SUpdateRenderWorldModels>();
        mWorld.ForceRun<SUpdateRenderWorldUI>();
        mWorld.ForceRun<SUpdateRenderWorldPointLights>();
        mWorld.ForceRun<SUpdateRenderWorldDirectionalLights>();
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void ProjectEmpty::OnGui()
    {
        if( ImGui::Begin("testoss"))
        {
            ImGui::End();
        }
    }
}