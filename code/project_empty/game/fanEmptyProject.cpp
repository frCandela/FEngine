#include "fanGameClient.hpp"

#include "core/time/fanProfiler.hpp"

#include "engine/singletons/fanScene.hpp"
#include "engine/components/fanCamera.hpp"
#include "engine/systems/fanUpdateRenderWorld.hpp"

#include "project_empty/game/components/fanTestComponent.hpp"
#include "project_empty/game/singletons/fanTestSingleton.hpp"

#include "project_empty/editor/fanRegisterEditorGui.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EmptyProject::Init()
	{
        EcsIncludeEngine( mWorld );
        EcsIncludePhysics(mWorld);
        EcsIncludeRender3D(mWorld);
        EcsIncludeRenderUI(mWorld);

        mWorld.AddComponentType<TestComponent>();
        mWorld.AddSingletonType<TestSingleton>();

        mName = "empty_project";

#ifdef FAN_EDITOR
        RegisterEditorGuiInfos( mWorld.GetSingleton<EditorGuiInfo>() );
#endif
	}

	//========================================================================================================
	//========================================================================================================
	void EmptyProject::Start()
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

	//========================================================================================================
	//========================================================================================================
	void  EmptyProject::Stop()
	{

	}

	//========================================================================================================
	//========================================================================================================
	void  EmptyProject::Step( const float /*_delta*/ )
	{
	}

    //========================================================================================================
    //========================================================================================================
    void EmptyProject::UpdateRenderWorld()
    {
        SCOPED_PROFILE( update_render_world );
        mWorld.ForceRun<SUpdateRenderWorldModels>();
        mWorld.ForceRun<SUpdateRenderWorldUI>();
        mWorld.ForceRun<SUpdateRenderWorldPointLights>();
        mWorld.ForceRun<SUpdateRenderWorldDirectionalLights>();
    }
}