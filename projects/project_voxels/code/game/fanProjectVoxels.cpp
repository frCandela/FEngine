#include "game/fanProjectVoxels.hpp"

#include "core/time/fanProfiler.hpp"
#include "core/fanDebug.hpp"
#include "core/math/fanFixedPoint.hpp"
#include "core/time/fanScopedTimer.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/components/fanCamera.hpp"
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
	    // creates a default camera
        Scene& scene = mWorld.GetSingleton<Scene>();
        SceneNode& cameraNode = scene.CreateSceneNode( "game_camera", &scene.GetRootNode() );
        cameraNode.AddFlag( SceneNode::NoSave | SceneNode::NoDelete | SceneNode::NoRaycast );

        const EcsEntity cameraID = mWorld.GetEntity( cameraNode.mHandle );
        Camera& camera = mWorld.AddComponent<Camera>( cameraID );
        camera.mType = Camera::Orthogonal;

        Transform& transform = mWorld.AddComponent<Transform>( cameraID );
        transform.SetRotationEuler( btVector3( 90.f, 0.f, 0.f ) );
        transform.SetPosition( btVector3( 0, 5, 0 ) );

        scene.SetMainCamera( cameraNode.mHandle );
	}

	//==========================================================================================================================
	//==========================================================================================================================
	void  ProjectVoxels::Stop()
	{

	}

	//==========================================================================================================================
	//==========================================================================================================================
	void  ProjectVoxels::Step( const float /*_delta*/ )
	{
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