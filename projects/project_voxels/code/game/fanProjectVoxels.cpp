#include "game/fanProjectVoxels.hpp"

#include "core/math/fanFixedPoint.hpp"
#include "core/math/fanQuaternion.hpp"
#include "core/time/fanScopedTimer.hpp"
#include "network/singletons/fanTime.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/systems/physics/fanUpdateFxRigidbodies.hpp"
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
#include "game/systems/fanTestSystem.hpp"

#include "editor/fanRegisterEditorGui.hpp"

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
        EcsIncludeEngine( mWorld );
        EcsIncludePhysics( mWorld );
        EcsIncludeRender3D( mWorld );
        EcsIncludeRenderUI( mWorld );

        mWorld.AddComponentType<TestComponent>();
        mWorld.AddSingletonType<TestSingleton>();

#ifdef FAN_EDITOR
        RegisterEditorGuiInfos( mWorld.GetSingleton<EditorGuiInfo>() );
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
            FxPhysicsWorld& fxPhysicsWorld = mWorld.GetSingleton<FxPhysicsWorld>();
            mWorld.Run<SIntegrateFxRigidbodies>( fxDelta, fxPhysicsWorld );
        }
        mWorld.Run<STestSystem>( fxDelta );

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

    btVector3 Meh( Vector3 _v )
    {
        return btVector3( _v.x.ToFloat(), _v.y.ToFloat(), _v.z.ToFloat() );
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