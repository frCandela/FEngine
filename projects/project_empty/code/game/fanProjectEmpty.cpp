#include "game/fanProjectEmpty.hpp"

#include "core/time/fanProfiler.hpp"
#include "core/fanDebug.hpp"

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
	void  ProjectEmpty::Stop()
	{

	}

	//==========================================================================================================================
	//==========================================================================================================================
	void  ProjectEmpty::Step( const float /*_delta*/ )
	{
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
        if( ImGui::Begin("my window"))
        {
            ImGui::End();
        }
    }
}