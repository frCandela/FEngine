#include "game/fanProjectEmpty.hpp"

#include "core/time/fanScopedTimer.hpp"
#include "network/singletons/fanTime.hpp"
#include "engine/systems/fanUpdateBounds.hpp"
#include "engine/systems/fanUpdateUIText.hpp"
#include "engine/systems/fanRaycastUI.hpp"
#include "engine/systems/fanUpdateUILayouts.hpp"
#include "engine/systems/fanUpdateUIAlign.hpp"
#include "engine/systems/fanUpdateTransforms.hpp"
#include "engine/systems/fanUpdateTimers.hpp"
#include "engine/systems/fanUpdateParticles.hpp"
#include "engine/systems/fanEmitParticles.hpp"
#include "engine/systems/fanGenerateParticles.hpp"
#include "engine/systems/fanUpdateRenderWorld.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/components/fanCamera.hpp"

#include "game/components/fanTestComponent.hpp"
#include "game/singletons/fanTestSingleton.hpp"


#ifdef FAN_EDITOR
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "editor/fanGuiTestSingleton.hpp"
#include "editor/fanGuiTestComponent.hpp"

#endif

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    ProjectEmpty::ProjectEmpty()
    {
        mName = "project_empty";
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ProjectEmpty::Init()
    {
        mWorld.AddComponentType<TestComponent>();
        mWorld.AddSingletonType<TestSingleton>();

#ifdef FAN_EDITOR
        EditorGuiInfo& editorGui = mWorld.GetSingleton<EditorGuiInfo>();
        editorGui.mSingletonInfos[TestSingleton::Info::sType] = GuiTestSingleton::GetInfo();
        editorGui.mComponentInfos[TestComponent::Info::sType] = GuiTestComponent::GetInfo();
#endif
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ProjectEmpty::Start()
    {
        MeshManager& meshManager = *mWorld.GetSingleton<RenderResources>().mMeshManager;
        RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
        meshManager.Add( renderWorld.mParticlesMesh, "particles_mesh_" + mName );

        Scene& scene = mWorld.GetSingleton<Scene>();
        SceneNode cameraNode = scene.CreateSceneNode( "game_camera", &scene.GetRootNode() );
        scene.SetMainCamera( cameraNode.mHandle );
        EcsEntity cameraEntity = mWorld.GetEntity( cameraNode.mHandle );
        mWorld.AddComponent<Transform>( cameraEntity );
        mWorld.AddComponent<Camera>( cameraEntity );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ProjectEmpty::Stop()
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ProjectEmpty::Step( const Fixed _delta )
    {
        SCOPED_PROFILE( step );
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
    void ProjectEmpty::Render()
    {
        SCOPED_PROFILE( update_render_world );

        RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
        renderWorld.drawData.clear();

        mWorld.ForceRun<SUpdateRenderWorldModels>( renderWorld );
        mWorld.ForceRun<SUpdateRenderWorldUI>();
        mWorld.ForceRun<SUpdateRenderWorldPointLights>();
        mWorld.ForceRun<SUpdateRenderWorldDirectionalLights>();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ProjectEmpty::OnGui()
    {
        if( ImGui::Begin( "testoss" ) )
        {
            ImGui::End();
        }
    }
}