#include "game/fanGameEmpty.hpp"

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
#include "engine/components/fanCamera.hpp"

#include "game/components/fanTestComponent.hpp"
#include "game/singletons/fanTestSingleton.hpp"

#ifdef FAN_EDITOR
#include "editor/singletons/fanEditorSettings.hpp"
#include "editor/fanGuiTestSingleton.hpp"
#include "editor/fanGuiTestComponent.hpp"

#endif

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GameEmpty::Init()
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
    void GameEmpty::Start()
    {
        Scene& scene = mWorld.GetSingleton<Scene>();
        SceneNode cameraNode = scene.CreateSceneNode( "game_camera", &scene.GetRootNode() );
        scene.SetMainCamera( cameraNode.mHandle );
        EcsEntity cameraEntity = mWorld.GetEntity( cameraNode.mHandle );
        mWorld.AddComponent<Transform>( cameraEntity );
        mWorld.AddComponent<Camera>( cameraEntity );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GameEmpty::Stop()
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GameEmpty::PreStep( const Fixed _delta )
    {
        (void)_delta;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GameEmpty::Step( const Fixed _delta )
    {
        SCOPED_PROFILE( step );
        (void)_delta;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GameEmpty::OnGui()
    {
        if( ImGui::Begin( "testoss" ) )
        {
        }
        ImGui::End();
    }
}