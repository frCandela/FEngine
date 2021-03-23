#include "game/fanProjectVoxels.hpp"

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
    ProjectVoxels::ProjectVoxels()
    {
        mName = "project_voxels";
    }

	//==========================================================================================================================
	//==========================================================================================================================
	void ProjectVoxels::Init()
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
	void ProjectVoxels::Start()
	{
        mWorld.Run<SRegisterAllRigidbodies>();
        MeshManager& meshManager = *mWorld.GetSingleton<RenderResources>().mMeshManager;
        RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
        meshManager.Add( renderWorld.mParticlesMesh, "particles_mesh_" + mName );
	}

	//==========================================================================================================================
	//==========================================================================================================================
	void  ProjectVoxels::Stop()
	{

	}

	//============================================================================================================================
    //	//========================================================================================================================
	void  ProjectVoxels::Step( const float _delta )
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
    void ProjectVoxels::UpdateRenderWorld()
    {
        SCOPED_PROFILE( update_render_world );
        mWorld.ForceRun<SUpdateRenderWorldModels>();
        mWorld.ForceRun<SUpdateRenderWorldUI>();
        mWorld.ForceRun<SUpdateRenderWorldPointLights>();
        mWorld.ForceRun<SUpdateRenderWorldDirectionalLights>();
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void ProjectVoxels::OnGui()
    {
        if( ImGui::Begin("testoss"))
        {
            static int num     = 10000;
            ImGui::DragInt("num", &num );
            if( ImGui::Button("test"))
            {
                Debug::Log() << "starting test" << Debug::Endl();
                Fixed     result1 = 0;
                {
                    ScopedTimer s( "test1" );
                    for( int    i = 0; i < num; ++i )
                    {
                        result1 += 1;
                    }
                }
                Fixed     result2 = 0;
                {
                    ScopedTimer s( "test2" );
                    for( int    i = 0; i < num; ++i )
                    {
                        result2 += Fixed("1.000000000");
                    }
                }
                Fixed     result3 = 0;
                {
                    ScopedTimer s( "test3" );
                    for( int    i = 0; i < num; ++i )
                    {
                        result3 += FIXED(1.000000000);
                    }
                }

                Debug::Log() << (result1 == result2 && result1 == result3 ) << Debug::Endl();
                fanAssert(result1 == result2 && result1 == result3);
            }
            ImGui::End();
        }
    }
}